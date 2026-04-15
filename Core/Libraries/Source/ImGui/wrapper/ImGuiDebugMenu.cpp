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
#include "ImGuiDebugMenu.h"
#include "imgui.h"

bool rts::ImGui::DebugMenu::s_showDemoWindow = false;
bool rts::ImGui::DebugMenu::s_showMetrics = false;
bool rts::ImGui::DebugMenu::s_showStyleEditor = false;

// Draw the master debug menu and any sub-windows toggled by the user.
// New debug windows are added by:
//   1. Adding a static bool above.
//   2. Adding a Checkbox in the master window below.
//   3. Adding the draw call gated by the bool at the bottom.
void rts::ImGui::DebugMenu::Draw()
{
    // Master window — always visible when the overlay is active.
    ::ImGui::SetNextWindowPos(ImVec2(10, 10), ImGuiCond_FirstUseEver);
    ::ImGui::SetNextWindowSize(ImVec2(280, 200), ImGuiCond_FirstUseEver);

    if (::ImGui::Begin("Debug Menu"))
    {
        ::ImGui::Text("ImGui %s", ::ImGui::GetVersion());
        ::ImGui::Text("FPS: %.1f (%.3f ms)", ::ImGui::GetIO().Framerate, 1000.0f / ::ImGui::GetIO().Framerate);
        ::ImGui::Separator();

        ::ImGui::Checkbox("Demo Window", &s_showDemoWindow);
        ::ImGui::Checkbox("Metrics / Debugger", &s_showMetrics);
        ::ImGui::Checkbox("Style Editor", &s_showStyleEditor);
    }
    ::ImGui::End();

    // Sub-windows. Each bool pointer lets the user close the window via its X button.
    if (s_showDemoWindow)
        ::ImGui::ShowDemoWindow(&s_showDemoWindow);

    if (s_showMetrics)
        ::ImGui::ShowMetricsWindow(&s_showMetrics);

    if (s_showStyleEditor)
    {
        ::ImGui::Begin("Style Editor", &s_showStyleEditor);
        ::ImGui::ShowStyleEditor();
        ::ImGui::End();
    }
}
