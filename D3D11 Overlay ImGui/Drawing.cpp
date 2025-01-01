#include "Drawing.h"

#include <string>
#include <unordered_map>
#include <format>
#include <filesystem>
#include <iostream>
#include <memory>
#include <windows.h>
#include <vector>

LPCSTR Drawing::lpWindowName = "Naughty Dog Debug Menu but ImGui";
ImVec2 Drawing::vWindowSize = { 0, 0 };
ImGuiWindowFlags Drawing::WindowFlags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize;
bool Drawing::bDraw = true;
ImFont* Drawing::pFont = nullptr; // Initialize to nullptr, will be set after AddFontFromFileTTF
UI::WindowItem Drawing::lpSelectedWindow; //Initialize the static variable.
std::string Drawing::buildDate = __DATE__ " " __TIME__; // Initialize build date.


std::unordered_map<std::string, int> selectedIndices;
std::string currentCategory = "";

std::unordered_map<std::string, std::unordered_map<std::string, bool>> submenuOptionStates;

enum class MenuCategory
{
    None,
    Display,
    Rendering,
    Engine,
    PlayerMenu,
    Weapons,
    GUI,
    Gameplay,
    LEDKeyboardEffects,
    Melee,
    Levels,
    NavigatingCharacter,
    Camera,
    Cinematics,
    Tasks,
    Example,
};

std::unordered_map<std::string, MenuCategory> menuCategoryMap = {
    {"Display...", MenuCategory::Display},
    {"Rendering...", MenuCategory::Rendering},
    {"Engine...", MenuCategory::Engine},
    {"Player Menu...", MenuCategory::PlayerMenu},
    {"Weapons...", MenuCategory::Weapons},
    {"GUI...", MenuCategory::GUI},
    {"Gameplay...", MenuCategory::Gameplay},
    {"LED Keyboard Effects...", MenuCategory::LEDKeyboardEffects},
    {"Melee...", MenuCategory::Melee},
    {"Levels...", MenuCategory::Levels},
    {"Navigating Character...", MenuCategory::NavigatingCharacter},
    {"Camera...", MenuCategory::Camera},
    {"Cinematics...", MenuCategory::Cinematics},
    {"Tasks...", MenuCategory::Tasks}
};

std::vector<std::string> mainMenuItems = {
    "Display...",
    "Rendering...",
    "Engine...",
    "Player Menu...",
    "Weapons...",
    "GUI...",
    "Gameplay...",
    "LED Keyboard Effects...",
    "Melee...",
    "Levels...",
    "Navigating Character...",
    "Camera...",
    "Cinematics...",
    "Tasks..."
};

ImVec2 watermarkPos;
float buttonWidth;

std::unordered_map<MenuCategory, std::unique_ptr<char[]>> submenuInputs;

bool Drawing::isActive()
{
    return bDraw;
}

void HandleInput()
{
    if (currentCategory.empty())
    {
        int& selectedIndex = selectedIndices["main"];
        if (GetAsyncKeyState(VK_UP) & 1)
        {
            selectedIndex--;
            if (selectedIndex < 0)
                selectedIndex = static_cast<int>(mainMenuItems.size() - 1);
        }
        if (GetAsyncKeyState(VK_DOWN) & 1)
        {
            selectedIndex++;
            if (selectedIndex >= static_cast<int>(mainMenuItems.size()))
                selectedIndex = 0;
        }
        if (GetAsyncKeyState(VK_RIGHT) & 1)
        {
            currentCategory = mainMenuItems[selectedIndex];

        }
    }
    else
    {
        if (GetAsyncKeyState(VK_LEFT) & 1)
        {
            currentCategory = "";
        }
    }
}

void HandleSubmenuInput(const std::string& submenu, const std::vector<std::string>& options, int& selectedIndex)
{
    if (GetAsyncKeyState(VK_UP) & 1)
    {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = static_cast<int>(options.size() - 1);
    }

    if (GetAsyncKeyState(VK_DOWN) & 1)
    {
        selectedIndex++;
        if (selectedIndex >= static_cast<int>(options.size()))
            selectedIndex = 0;
    }

    if (GetAsyncKeyState(VK_RIGHT) & 1)
    {
        if (selectedIndex >= 0 && selectedIndex < options.size())
        {
            const std::string& selectedOption = options[selectedIndex];
            submenuOptionStates[submenu][selectedOption] = !submenuOptionStates[submenu][selectedOption];
        }
    }
}

void Drawing::Draw()
{
    ImGuiViewport* viewport = ImGui::GetMainViewport();
    watermarkPos = ImVec2(100, viewport->WorkSize.y - 100 - ImGui::GetFont()->FontSize);

    if (isActive())
    {
        HandleInput();

        ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.3f, 0.3f, 0.3f, 0.8f));
        ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_Separator, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 1.0f);

        ImGui::SetNextWindowSize(ImVec2(0, 0), ImGuiCond_Always);

        ImGui::Begin("Dev Menu", &bDraw, WindowFlags);
        {
            // Push the custom font for the whole menu window.
            if (pFont) ImGui::PushFont(pFont);

            ImGui::Text("Dev Menu");
            ImGui::Separator();

            if (currentCategory.empty())
            {
                int selectedIndex = selectedIndices["main"];
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
                for (size_t i = 0; i < mainMenuItems.size(); i++)
                {
                    if (i == selectedIndex)
                        ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), " >");
                    else
                        ImGui::Text("  ");

                    ImGui::SameLine();
                    ImGui::Text("%s", mainMenuItems[i].c_str());
                }
                ImGui::PopStyleVar();
                ImGui::Separator();
                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.6f, 0.6f, 1.0f));
                buttonWidth = ImGui::GetContentRegionAvail().x;
                ImGui::SetCursorPosX((buttonWidth - ImGui::CalcTextSize("Exit Debug Menu").x) * 0.5f);

                if (ImGui::Button("Exit Debug Menu"))
                {
                    exit(0);
                }
                ImGui::PopStyleColor(3);
            }
            else
            {
                ImGui::Text("Currently in category:");
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "%s", currentCategory.c_str());
                ImGui::NewLine();

                switch (menuCategoryMap[currentCategory])
                {
                case MenuCategory::Display:
                {
                    ImGui::Text("Display Menu Options");

                    const std::string& submenu = currentCategory;
                    const std::vector<std::string> options = {
                        "Show FPS",
                        "Show Resolution",
                        "Show Build Date"
                    };

                    static std::unordered_map<std::string, int> submenuSelectedIndices;
                    int& submenuSelectedIndex = submenuSelectedIndices[submenu];

                    HandleSubmenuInput(submenu, options, submenuSelectedIndex);

                    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 4));
                    for (size_t i = 0; i < options.size(); i++)
                    {
                        const std::string& option = options[i];
                        bool& optionState = submenuOptionStates[submenu][option];

                        if (static_cast<int>(i) == submenuSelectedIndex)
                        {
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), " >");
                        }
                        else
                        {
                            ImGui::Text("  ");
                        }
                        ImGui::SameLine();

                        ImGui::Text("%s: ", option.c_str());
                        ImGui::SameLine();

                        if (optionState)
                            ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "ON");
                        else
                            ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "OFF");
                    }
                    ImGui::PopStyleVar();
                    break;
                }
                case MenuCategory::Rendering:
                    ImGui::Text("Rendering Menu Options");
                    break;
                case MenuCategory::Engine:
                    ImGui::Text("Engine Menu Options");
                    break;
                case MenuCategory::PlayerMenu:
                    ImGui::Text("Player Menu Options");
                    break;
                case MenuCategory::Weapons:
                    ImGui::Text("Weapons Menu Options");
                    break;
                case MenuCategory::GUI:
                    ImGui::Text("GUI Menu Options");
                    break;
                case MenuCategory::Gameplay:
                    ImGui::Text("Gameplay Menu Options");
                    break;
                case MenuCategory::LEDKeyboardEffects:
                    ImGui::Text("LEDKeyboardEffects Menu Options");
                    break;
                case MenuCategory::Melee:
                    ImGui::Text("Melee Menu Options");
                    break;
                case MenuCategory::Levels:
                    ImGui::Text("Levels Menu Options");
                    break;
                case MenuCategory::NavigatingCharacter:
                    ImGui::Text("NavigatingCharacter Menu Options");
                    break;
                case MenuCategory::Camera:
                    ImGui::Text("Camera Menu Options");
                    break;
                case MenuCategory::Cinematics:
                    ImGui::Text("Cinematics Menu Options");
                    break;
                case MenuCategory::Tasks:
                    ImGui::Text("Tasks Menu Options");
                    break;
                default:
                    ImGui::Text("Use Left Arrow to go back.");
                }
            }
            if (pFont) ImGui::PopFont();
        }
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }

    if (pFont) ImGui::PushFont(pFont);
    ImGui::GetForegroundDrawList()->AddText(watermarkPos, ImColor(255, 255, 255), "Naughty Dog - Debug Menu ImGui Port by hoppers");
    float offset = ImGui::GetFont()->FontSize;
    if (submenuOptionStates["Display..."]["Show FPS"])
    {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(watermarkPos.x, watermarkPos.y + offset), ImColor(255, 255, 255), std::format("Application average {:.3f} ms/frame ({:.1f} FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate).c_str());
        offset += ImGui::GetFont()->FontSize;
    }

    if (submenuOptionStates["Display..."]["Show Resolution"])
    {
        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::GetForegroundDrawList()->AddText(ImVec2(watermarkPos.x, watermarkPos.y + offset), ImColor(255, 255, 255), std::format("Resolution: {}x{}", viewport->WorkSize.x, viewport->WorkSize.y).c_str());
        offset += ImGui::GetFont()->FontSize;
    }

    if (submenuOptionStates["Display..."]["Show Build Date"])
    {
        ImGui::GetForegroundDrawList()->AddText(ImVec2(watermarkPos.x, watermarkPos.y + offset), ImColor(255, 255, 255), std::format("Build Date: {}", buildDate).c_str());
    }

    if (pFont) ImGui::PopFont();

    if (GetAsyncKeyState(VK_INSERT) & 1)
        bDraw = !bDraw;
}