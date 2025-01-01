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

std::unordered_map<std::string, int> selectedIndices;
std::string currentCategory = "";

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

ImFont* customFont = nullptr;
std::string currentFontName = "Default";
bool usingCustomFont = false;
bool showWatermark = true;

ImVec2 watermarkPos;
float buttonWidth;

std::unordered_map<MenuCategory, std::unique_ptr<char[]>> submenuInputs;

bool Drawing::isActive()
{
    return bDraw;
}

void HandleInput()
{
    int& selectedIndex = selectedIndices[currentCategory];

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
        if (currentCategory.empty())
        {
            currentCategory = mainMenuItems[selectedIndex];
            selectedIndices[currentCategory] = 0;
        }
    }
    if (GetAsyncKeyState(VK_LEFT) & 1)
    {
        if (!currentCategory.empty())
        {
            currentCategory = "";
        }
    }
}

void Drawing::Draw()
{
    ImGui::PushFont(customFont != nullptr && usingCustomFont ? customFont : ImGui::GetFont());
    if (customFont != nullptr && usingCustomFont)
        ImGui::GetFont()->Scale = 1.0f;
    else
        ImGui::GetFont()->Scale = 0.8f;

    ImGuiViewport* viewport = ImGui::GetMainViewport();
    watermarkPos = ImVec2(100, viewport->WorkSize.y - 100 - ImGui::GetFontSize());

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
            ImGui::Text("Dev Menu");
            ImGui::Separator();

            int selectedIndex = selectedIndices[currentCategory];
            if (currentCategory.empty())
            {
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
                    ImGui::Text("Dispaly Menu Options");
                    break;
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
        }
        ImGui::End();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);
    }
    ImGui::GetForegroundDrawList()->AddText(watermarkPos, ImColor(255, 255, 255), "Naughty Dog - Debug Menu ImGui Port by hoppers");

    ImGui::PopFont();

    if (GetAsyncKeyState(VK_INSERT) & 1)
        bDraw = !bDraw;
}