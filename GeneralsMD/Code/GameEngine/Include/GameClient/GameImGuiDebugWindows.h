/*
**	Command & Conquer Generals Zero Hour(tm)
**	Copyright 2026 TheSuperHackers
**	Authors: zhp47
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

#pragma once

#ifdef RTS_HAS_IMGUI

// TheSuperHackers @feature zhp47 15/04/2026
// Game-specific ImGui debug windows that expose the existing META DEMO debug
// commands through a discoverable tabbed interface. Each tab mirrors a category
// of debug features that were previously only accessible via keyboard shortcuts.

namespace GameImGuiDebugWindows
{

// Draw all game debug windows. Registered as a callback on
// rts::ImGui::DebugMenu so it runs during the render phase.
void Draw();

// Register the Draw callback with the master debug menu.
// Call once during game initialization (e.g. GameClient::init).
void Register();

} // namespace GameImGuiDebugWindows

#endif // RTS_HAS_IMGUI
