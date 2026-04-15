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

// Owns the ImGui context and both platform/renderer backends.
// Exactly one instance should exist for the lifetime of ImGui usage (typically
// a static in dx8wrapper.cpp). Call Init() once with the Win32 HWND and the
// IDirect3DDevice8 pointer. The destructor tears everything down in the
// correct reverse order: renderer backend -> platform backend -> context.
class ContextManager
{
  public:
    ContextManager();
    ~ContextManager();

    // Set up the ImGui context and both backends. Safe to call only once;
    // subsequent calls are no-ops. hwnd = the game's main HWND, device = IDirect3DDevice8*.
    void Init(void *hwnd, void *device);

  private:
    bool m_initialized;

    // TODO change this to rule of 5 and use delete instead of declaring without
    // definition once we are using a C++11 or higher C++ Standard
    ContextManager(const ContextManager &);
    ContextManager &operator=(const ContextManager &);
};

} // namespace ImGui
} // namespace rts
