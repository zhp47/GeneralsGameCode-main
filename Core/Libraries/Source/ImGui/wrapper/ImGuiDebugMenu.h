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

#pragma once

namespace rts
{
namespace ImGui
{

// Central hub for all ImGui debug windows. Call Draw() once per frame between
// BeginFrame/EndFrame. Renders the master menu and any sub-windows the user
// has enabled via checkboxes.
//
// Game-specific debug windows live in the GameEngine layer. They register a
// draw callback via SetGameDrawCallback() and DebugMenu calls it each frame.
class DebugMenu
{
  public:
    using DrawCallback = void(*)();

    // Register a function that draws game-specific ImGui debug windows.
    // Called once from GameEngine init. The callback runs inside the master
    // menu's frame scope whenever the overlay is visible.
    static void SetGameDrawCallback(DrawCallback cb);

    static void Draw();

  private:
    static DrawCallback s_gameDrawCallback;
    static bool s_showGameDebug;
    static bool s_showDemoWindow;
    static bool s_showMetrics;
    static bool s_showStyleEditor;
};

} // namespace ImGui
} // namespace rts
