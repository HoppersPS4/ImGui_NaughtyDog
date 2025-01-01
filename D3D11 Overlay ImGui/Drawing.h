#ifndef DRAWING_H
#define DRAWING_H

#include "pch.h"
#include "UI.h"
#include <string>
#include <unordered_map>
#include <vector>

enum class MenuCategory;

class Drawing
{
private:
    static LPCSTR lpWindowName;
    static ImVec2 vWindowSize;
    static ImGuiWindowFlags WindowFlags;
    static bool bDraw;
    static UI::WindowItem lpSelectedWindow;
    static std::string buildDate; // Added for build date.


public:
    static bool isActive();
    static void Draw();
    static ImFont* pFont; // Added font pointer

};

void HandleInput();
void HandleSubmenuInput(const std::string& submenu, const std::vector<std::string>& options, int& selectedIndex);

#endif