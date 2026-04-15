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
#include "ImGuiFrameManager.h"
#include "imgui.h"
#include "imgui_impl_dx8.h"
#include "imgui_impl_win32.h"

// Track whether a frame is currently open to prevent re-entrant or duplicate frames.
bool rts::ImGui::FrameManager::s_frameOpen = false;

// Backend NewFrame() calls must happen in this order:
// 1. Renderer (DX8) -- may recreate GPU resources.
// 2. Platform (Win32) -- updates input state.
// 3. ImGui::NewFrame() -- starts accepting UI commands for this frame.
void rts::ImGui::FrameManager::BeginFrame()
{
    if (s_frameOpen)
    {
        return;
    }

    ImGui_ImplDX8_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ::ImGui::NewFrame();

    s_frameOpen = true;
}

// Finalize the current frame: ImGui::Render() produces the draw lists,
// then RenderDrawData sends them through our DX8 backend.
void rts::ImGui::FrameManager::EndFrame()
{
    if (!s_frameOpen)
    {
        return;
    }

    ::ImGui::Render();

    ImDrawData *data = ::ImGui::GetDrawData();
    if (data && data->CmdListsCount > 0)
    {
        ImGui_ImplDX8_RenderDrawData(data);
    }

    s_frameOpen = false;
}
