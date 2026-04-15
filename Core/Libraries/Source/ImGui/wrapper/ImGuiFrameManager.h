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

// Static helpers that bracket each ImGui frame.
// BeginFrame() calls NewFrame() on both backends and then ImGui::NewFrame().
// EndFrame() calls ImGui::Render() and dispatches the resulting draw data
// to the DX8 renderer backend.
// A static guard (s_frameOpen) prevents nested or duplicate frames.
class FrameManager
{
  public:
    // Start a new ImGui frame. Must be matched by EndFrame() before the next call.
    static void BeginFrame();
    // Finish the current frame: render draw data via the DX8 backend.
    static void EndFrame();

  private:
    static bool s_frameOpen;
};

/*
 * FrameGuard -- RAII wrapper around BeginFrame/EndFrame.
 * Construct at the top of a scope to open a frame; the destructor ends it.
 */
class FrameGuard
{
  public:
    FrameGuard() { FrameManager::BeginFrame(); }

    ~FrameGuard() { FrameManager::EndFrame(); }

    // TODO change this to rule of 5 and use delete instead of declaring without
    // definition once we are using a C++11 or higher C++ Standard
  private:
    FrameGuard(const FrameGuard &);
    FrameGuard &operator=(const FrameGuard &);
};
} // namespace ImGui
} // namespace rts