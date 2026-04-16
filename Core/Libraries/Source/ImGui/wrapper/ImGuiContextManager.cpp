/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2026 TheSuperHackers
**	Authors: jurassicLizard (Salem B.), zhp47
**
**	This program is free software: you can redistribute it and/or modify
**	it under the terms of the GNU General Public License as published by
**	the Free Software Foundation, either version 3 of the License, or
**	(at your option) any later version.
**
**	This program is distributed in the hope that it will be useful,
**	but WITHOUT ANY WARRANTY; without even the implied warranty of
**	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**	GNU General Public License for more details.
**
**	You should have received a copy of the GNU General Public License
**	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ImGuiContextManager.h"
#include "imgui.h"
#include "imgui_impl_dx8.h"
#include "imgui_impl_win32.h"
#include <d3d8.h>
#include <windows.h>

rts::ImGui::ContextManager::ContextManager() : m_initialized(false) {}

// Shutdown backends and destroy the ImGui context. Order matters:
// renderer (DX8) first, then platform (Win32), then the context itself.
rts::ImGui::ContextManager::~ContextManager()
{
    if (m_initialized)
    {
        ImGui_ImplDX8_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ::ImGui::DestroyContext();
    }
}

// One-time setup. Creates the ImGui context, configures input flags,
// applies the dark color scheme, initialises both backends, and adds
// the default font to the atlas.
void rts::ImGui::ContextManager::Init(void *hwnd, void *device)
{
    if (m_initialized)
    {
        return;
    }

    ::IMGUI_CHECKVERSION();
    ::ImGui::CreateContext();
    ImGuiIO &io = ::ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;

    ::ImGui::StyleColorsDark();

    // TheSuperHackers @feature zhp47 15/04/2026 Apply Moonlight theme (by Madam-Herta from ImThemes).
    {
        ImGuiStyle &style = ::ImGui::GetStyle();
        style.Alpha = 1.0f;
        style.DisabledAlpha = 1.0f;
        style.WindowPadding = ImVec2(12.0f, 12.0f);
        style.WindowRounding = 11.5f;
        style.WindowBorderSize = 0.0f;
        style.WindowMinSize = ImVec2(20.0f, 20.0f);
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
        style.WindowMenuButtonPosition = ImGuiDir_Right;
        style.ChildRounding = 0.0f;
        style.ChildBorderSize = 1.0f;
        style.PopupRounding = 0.0f;
        style.PopupBorderSize = 1.0f;
        style.FramePadding = ImVec2(20.0f, 3.4f);
        style.FrameRounding = 11.9f;
        style.FrameBorderSize = 0.0f;
        style.ItemSpacing = ImVec2(4.3f, 5.5f);
        style.ItemInnerSpacing = ImVec2(7.1f, 1.8f);
        style.CellPadding = ImVec2(12.1f, 9.2f);
        style.IndentSpacing = 0.0f;
        style.ColumnsMinSpacing = 4.9f;
        style.ScrollbarSize = 11.6f;
        style.ScrollbarRounding = 15.9f;
        style.GrabMinSize = 3.7f;
        style.GrabRounding = 20.0f;
        style.TabRounding = 0.0f;
        style.TabBorderSize = 0.0f;
        style.ColorButtonPosition = ImGuiDir_Right;
        style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign = ImVec2(0.0f, 0.0f);

        ImVec4 *colors = style.Colors;
        colors[ImGuiCol_Text]                  = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_TextDisabled]          = ImVec4(0.27f, 0.32f, 0.45f, 1.0f);
        colors[ImGuiCol_WindowBg]              = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_ChildBg]               = ImVec4(0.09f, 0.10f, 0.12f, 1.0f);
        colors[ImGuiCol_PopupBg]               = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_Border]                = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
        colors[ImGuiCol_BorderShadow]          = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_FrameBg]               = ImVec4(0.11f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_FrameBgHovered]        = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
        colors[ImGuiCol_FrameBgActive]         = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
        colors[ImGuiCol_TitleBg]               = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
        colors[ImGuiCol_TitleBgActive]         = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
        colors[ImGuiCol_TitleBgCollapsed]      = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_MenuBarBg]             = ImVec4(0.10f, 0.11f, 0.12f, 1.0f);
        colors[ImGuiCol_ScrollbarBg]           = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
        colors[ImGuiCol_ScrollbarGrab]         = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabHovered]  = ImVec4(0.16f, 0.17f, 0.19f, 1.0f);
        colors[ImGuiCol_ScrollbarGrabActive]   = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_CheckMark]             = ImVec4(0.97f, 1.0f, 0.50f, 1.0f);
        colors[ImGuiCol_SliderGrab]            = ImVec4(0.97f, 1.0f, 0.50f, 1.0f);
        colors[ImGuiCol_SliderGrabActive]      = ImVec4(1.0f, 0.80f, 0.50f, 1.0f);
        colors[ImGuiCol_Button]                = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_ButtonHovered]         = ImVec4(0.18f, 0.19f, 0.20f, 1.0f);
        colors[ImGuiCol_ButtonActive]          = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        colors[ImGuiCol_Header]                = ImVec4(0.14f, 0.16f, 0.21f, 1.0f);
        colors[ImGuiCol_HeaderHovered]         = ImVec4(0.11f, 0.11f, 0.11f, 1.0f);
        colors[ImGuiCol_HeaderActive]          = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_Separator]             = ImVec4(0.13f, 0.15f, 0.19f, 1.0f);
        colors[ImGuiCol_SeparatorHovered]      = ImVec4(0.16f, 0.18f, 0.25f, 1.0f);
        colors[ImGuiCol_SeparatorActive]       = ImVec4(0.16f, 0.18f, 0.25f, 1.0f);
        colors[ImGuiCol_ResizeGrip]            = ImVec4(0.15f, 0.15f, 0.15f, 1.0f);
        colors[ImGuiCol_ResizeGripHovered]     = ImVec4(0.97f, 1.0f, 0.50f, 1.0f);
        colors[ImGuiCol_ResizeGripActive]      = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        colors[ImGuiCol_Tab]                   = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_TabHovered]            = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_TabActive]             = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_TabUnfocused]          = ImVec4(0.08f, 0.09f, 0.10f, 1.0f);
        colors[ImGuiCol_TabUnfocusedActive]    = ImVec4(0.13f, 0.27f, 0.57f, 1.0f);
        colors[ImGuiCol_PlotLines]             = ImVec4(0.52f, 0.60f, 0.70f, 1.0f);
        colors[ImGuiCol_PlotLinesHovered]      = ImVec4(0.04f, 0.98f, 0.98f, 1.0f);
        colors[ImGuiCol_PlotHistogram]         = ImVec4(0.88f, 0.80f, 0.56f, 1.0f);
        colors[ImGuiCol_PlotHistogramHovered]  = ImVec4(0.96f, 0.96f, 0.96f, 1.0f);
        colors[ImGuiCol_TableHeaderBg]         = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
        colors[ImGuiCol_TableBorderStrong]     = ImVec4(0.05f, 0.05f, 0.07f, 1.0f);
        colors[ImGuiCol_TableBorderLight]      = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        colors[ImGuiCol_TableRowBg]            = ImVec4(0.12f, 0.13f, 0.15f, 1.0f);
        colors[ImGuiCol_TableRowBgAlt]         = ImVec4(0.10f, 0.11f, 0.12f, 1.0f);
        colors[ImGuiCol_TextSelectedBg]        = ImVec4(0.94f, 0.94f, 0.94f, 1.0f);
        colors[ImGuiCol_DragDropTarget]        = ImVec4(0.50f, 0.51f, 1.0f, 1.0f);
        colors[ImGuiCol_NavHighlight]          = ImVec4(0.27f, 0.29f, 1.0f, 1.0f);
        colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.50f, 0.51f, 1.0f, 1.0f);
        colors[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.20f, 0.18f, 0.55f, 0.50f);
        colors[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.20f, 0.18f, 0.55f, 0.50f);
    }

    // TheSuperHackers @bugfix zhp47 15/04/2026 Check backend init return values
    // to avoid calling Shutdown on uninitialized backends in the destructor.
    bool win32Ok = ImGui_ImplWin32_Init(static_cast<HWND>(hwnd));
    bool dx8Ok = ImGui_ImplDX8_Init(static_cast<IDirect3DDevice8 *>(device));
    if (!win32Ok || !dx8Ok)
    {
        if (dx8Ok)
            ImGui_ImplDX8_Shutdown();
        if (win32Ok)
            ImGui_ImplWin32_Shutdown();
        ::ImGui::DestroyContext();
        return;
    }

    // TheSuperHackers @tweak zhp47 15/04/2026 Use a larger default font (18px instead of 13px)
    // for readability in windowed tools like WorldBuilder where the backbuffer may be stretched.
    ImFontConfig fontCfg;
    fontCfg.SizePixels = 18.0f;
    io.Fonts->AddFontDefault(&fontCfg);

    m_initialized = true;
}
