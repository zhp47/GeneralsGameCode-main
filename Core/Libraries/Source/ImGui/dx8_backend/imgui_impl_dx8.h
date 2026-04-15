/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2014 - 2025 Omar Cornut (Based on dx9 ImGui Backend)
 * Copyright (c) 2025 Meigyoku-Thmn and others
 * Copyright (c) 2026 TheSuperHackers (jurassicLizard, zhp47)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

// dear imgui: Renderer Backend for DirectX8
// This needs to be used along with a Platform Backend (e.g. Win32)

// Implemented features:
//  [X] Renderer: User texture binding. Use 'LPDIRECT3DTEXTURE8' as ImTextureID. Read the FAQ about ImTextureID!
//  [X] Renderer: Large meshes support (64k+ vertices) with 16-bit indices.

// You can use unmodified imgui_impl_* files in your project. See examples/ folder for examples of using this.
// Prefer including the entire imgui/ repository into your project (either as a copy or as a submodule), and only build
// the backends you need. Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#pragma once
#include <imgui.h> // IMGUI_IMPL_API

struct IDirect3DDevice8;

// Called once during engine init. Stores the D3D8 device for later use.
IMGUI_IMPL_API bool ImGui_ImplDX8_Init(IDirect3DDevice8 *device);
// Called once during engine shutdown. Frees all GPU resources and backend data.
IMGUI_IMPL_API void ImGui_ImplDX8_Shutdown();
// Called at the start of each frame. Re-creates GPU resources if they were invalidated.
IMGUI_IMPL_API void ImGui_ImplDX8_NewFrame();
// Renders the ImGui draw data using DX8 draw calls. Call after ImGui::Render().
IMGUI_IMPL_API void ImGui_ImplDX8_RenderDrawData(ImDrawData *draw_data);

// Called by InvalidateDeviceObjects/CreateDeviceObjects around a D3D device Reset().
IMGUI_IMPL_API bool ImGui_ImplDX8_CreateDeviceObjects();
IMGUI_IMPL_API void ImGui_ImplDX8_InvalidateDeviceObjects();
