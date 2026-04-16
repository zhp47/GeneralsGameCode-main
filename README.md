## Dear ImGui DX8 Integration

> **Branch:** `feature/imgui-dx8-integration` · **Base:** `main` · **Status:** Working (Game + WorldBuilder, Debug + Release)

This branch integrates [Dear ImGui](https://github.com/ocornut/imgui) 1.92.6 WIP into C&C Generals / Zero Hour using a custom DirectX 8 renderer backend. The overlay is toggled with **F11** and is gated behind the `RTS_BUILD_OPTION_IMGUI` / `RTS_HAS_IMGUI` compile flag, so it has zero impact on builds when disabled. All changes are applied identically to both Generals and GeneralsMD (Zero Hour).

### Features

#### Master Debug Menu
- FPS counter with frame time display
- Checkboxes to toggle Demo Window, Metrics/Debugger, Style Editor
- Game Debug panel toggle

#### Camera Tab *(Release + Debug)*
- Max/min camera height sliders
- Pitch and rotation sliders
- Field of view slider
- Horizontal, vertical, and keyboard scroll speed sliders
- Zoom limit toggle
- Reset all camera settings to defaults
- *(Debug only)* Debug camera mode, lock to selection, lock to planes, zoom lock, time of day cycle

#### Rendering Tab *(Release + Debug)*
- Shadow volumes toggle
- *(Debug only)* Behind-building markers toggle
- *(Debug only)* Trackmarks, water plane, feather water toggles
- *(Debug only)* BW/wireframe, red, green color filters
- *(Debug only)* Motion blur zoom toggle
- *(Debug only)* LOD decrease, increase, and cycle controls
- *(Debug only)* Letterbox and message text toggles

#### Cheats Tab *(Debug only)*
- Instant build, free build, ignore prerequisites toggles (per human player)
- Special power delay toggle
- Add $10,000 button
- Give all sciences, give science purchase points
- Manual rank level adjust (+/-)
- Hand of God mode (one-hit kills)
- Hurt Me mode (take 10% HP per hit)
- Kill selected units, kill all enemies
- Veterancy promote/demote
- Switch teams, switch China/USA
- Instant win
- Multiplayer detection — most cheats auto-locked in MP games

#### Debug Displays Tab *(Debug only)*
- Graphical framerate bar
- Engine debug stats overlay
- Particle, threat, vision, projectile, cash map debug visualizations
- Supply center placement debug *(Zero Hour only)*
- AI debug level cycling
- Selection debug, show extents, show health
- Show audio locations *(Zero Hour only)*

#### Audio Tab *(Debug only)*
- Master sound toggle
- Music toggle, next/prev track
- Military subtitles toggle
- Audio debug toggle

#### Scripts & Diagnostics Tab *(Debug only)*
- Run map scripts 1–9
- Dump assets, dump player objects, dump all player objects
- Performance statistical dump
- Next objective movie, play cameo movie
- AVI capture toggle

#### WorldBuilder Integration
- F11 works globally from any panel, toolbar, or child window (not just the 3D viewport)
- Overlay renders crisp at any window size — backbuffer resizes dynamically when maximized/restored
- Mouse input doesn't fight with editor tools — coordinates scaled correctly from HWND to backbuffer space
- Deferred device reset avoids DX8 asserts during MFC window creation

### Build

```bash
# Release build with ImGui:
cmake --preset win32
cmake --build build/win32 --config Release --target generalszh.exe WorldBuilderZH.exe

# Debug build:
cmake --preset win32-debug
cmake --build build/win32-debug --config Debug --target WorldBuilderZH.exe
```

ImGui is enabled by default when `RTS_BUILD_OPTION_IMGUI=ON` (set in presets).

### Usage

| Key / Input | Action |
|---|---|
| **F11** | Toggle ImGui debug overlay on/off |
| **Mouse** | Drag, resize, click ImGui windows |
| **Keyboard** | Type in ImGui input fields when overlay has focus |

When the overlay is visible, mouse and keyboard events are consumed by ImGui and do not pass through to WorldBuilder tools. When hidden, zero overhead — no ImGui frames are processed.

### Files Changed (12 files)

| File | Description |
|---|---|
| `Core/Libraries/Source/ImGui/dx8_backend/imgui_impl_dx8.cpp` | DX8 renderer backend |
| `Core/Libraries/Source/ImGui/dx8_backend/imgui_impl_dx8.h` | Public API declarations |
| `Core/Libraries/Source/ImGui/wrapper/ImGuiContextManager.cpp` | Context init / font config |
| `Core/Libraries/Source/ImGui/wrapper/ImGuiFrameManager.cpp` | Frame begin/end orchestration |
| `Generals/Code/Tools/WorldBuilder/include/MainFrm.h` | F11 toggle declaration |
| `Generals/Code/Tools/WorldBuilder/include/wbview3d.h` | Pending resize members |
| `Generals/Code/Tools/WorldBuilder/src/MainFrm.cpp` | F11 PreTranslateMessage handler |
| `Generals/Code/Tools/WorldBuilder/src/wbview3d.cpp` | Input scaling + deferred resize |
| `GeneralsMD/Code/Tools/WorldBuilder/include/MainFrm.h` | (same as Generals) |
| `GeneralsMD/Code/Tools/WorldBuilder/include/wbview3d.h` | (same as Generals) |
| `GeneralsMD/Code/Tools/WorldBuilder/src/MainFrm.cpp` | (same as Generals) |
| `GeneralsMD/Code/Tools/WorldBuilder/src/wbview3d.cpp` | (same as Generals) |

### Detailed Changelog

#### imgui_impl_dx8.cpp / imgui_impl_dx8.h

1. **`ImGui_ImplDX8_AdjustDisplaySize()`** — New function. Called after `ImGui_ImplWin32_NewFrame()` and before `ImGui::NewFrame()`. Queries the actual DX8 backbuffer dimensions via `GetRenderTarget()`, overrides `io.DisplaySize` to match, caches HWND-to-backbuffer scale factors, and injects a fresh scaled mouse position event using `GetCursorPos`/`ScreenToClient`. Reading the cursor directly from the OS is critical because `io.MousePos` holds the previous frame's already-scaled value — rescaling it would compound the scale factor every frame, causing coordinates to exponentially shrink toward (0,0).

2. **`ImGui_ImplDX8_GetInputScale()`** — New function. Returns the cached HWND-to-backbuffer scale factors so call sites (`WbView3d::WindowProc`) can transform mouse coordinates before they enter ImGui's event queue.

3. **`SetupRenderState()` viewport + projection fix** — Viewport now uses `DisplaySize` (clamped to backbuffer bounds) instead of always using the render target dimensions. Orthographic projection matrix uses `DisplaySize` for correct coordinate mapping when backbuffer ≠ window size.

4. **`NewFrame()` depth/stencil buffer resize** — Detects when the backbuffer dimensions have changed (after a device Reset) and recreates the dedicated ImGui depth/stencil surface to match.

5. **`InputScaleX` / `InputScaleY`** — New members in `ImGui_ImplDX8_Data`, initialized to 1.0f, updated each frame by `AdjustDisplaySize`.

#### ImGuiContextManager.cpp

6. **Font size increased from 13px to 18px** — The default ImGui font at 13px was too small in WorldBuilder where the backbuffer may be stretched.

#### ImGuiFrameManager.cpp

7. **`BeginFrame()` calls `ImGui_ImplDX8_AdjustDisplaySize()`** — Inserted between `ImGui_ImplWin32_NewFrame()` and `ImGui::NewFrame()` to sync DisplaySize with the actual backbuffer.

#### MainFrm.h / MainFrm.cpp (both games)

8. **F11 toggle moved to `CMainFrame::PreTranslateMessage()`** — Previously handled in `WbView3d::WindowProc` (only fires when 3D viewport has focus). Now works regardless of which toolbar, panel, or child widget has keyboard focus.

#### wbview3d.h / wbview3d.cpp (both games)

9. **Deferred backbuffer resize** — `OnSize()` stores pending dimensions; `redraw()` processes them at frame start (after `m_ww3dInited` is confirmed true) via `reset3dEngineDisplaySize()` → `Set_Device_Resolution()` → `Reset_Device()`. This avoids calling `Reset_Device` during MFC window creation when the DX8 device isn't ready.

10. **`WindowProc` mouse coordinate scaling** — Mouse messages (`WM_MOUSEFIRST..WM_MOUSELAST`) are scaled from HWND client space to backbuffer space using `ImGui_ImplDX8_GetInputScale()` before being forwarded to `ImGui_ImplWin32_WndProcHandler`. The F11 toggle was removed from here (moved to MainFrm).

### Bugs Fixed

| Bug | Cause | Fix |
|---|---|---|
| ImGui windows fight against drag/resize when maximized | `AdjustDisplaySize` re-scaled `io.MousePos` (already scaled from previous frame) every frame → exponential drift toward (0,0) | Read fresh cursor position from OS via `GetCursorPos`/`ScreenToClient` |
| Blurry/pixelated ImGui text when maximized | Backbuffer stayed at initial window size; DX8 stretched on `Present()` | Deferred `OnSize` → `reset3dEngineDisplaySize` in the render loop |
| DX8 debug assert (`WWASSERT(0)`) on WorldBuilder startup | `OnSize` called `Reset_Device` during MFC window creation before device ready | Deferred resize pattern — store pending size, apply in `redraw()` |
| F11 toggle didn't work when toolbar/panel had focus | `WbView3d::WindowProc` only receives messages when 3D view has focus | Moved F11 handling to `CMainFrame::PreTranslateMessage` |
| Stale depth/stencil buffer after backbuffer resize | ImGui's dedicated depth surface wasn't recreated after device reset | `NewFrame` checks backbuffer size and recreates depth buffer if mismatched |

### Architecture

The ImGui integration follows a layered design:

```
DX8 Backend (imgui_impl_dx8.cpp)
  └─ Custom DX8 renderer: vertex/index buffers, font atlas texture,
     stencil-based scissor clipping, fixed-function pipeline setup.
     Handles device reset via InvalidateDeviceObjects/CreateDeviceObjects.

Frame Manager (ImGuiFrameManager.cpp)
  └─ Orchestrates BeginFrame/EndFrame. Called from DX8Wrapper::Begin_Scene
     and End_Scene. Guards against re-entrant calls.

Context Manager (ImGuiContextManager.cpp)
  └─ One-time init: creates ImGui context, initializes Win32 + DX8 backends,
     configures font atlas.

Debug Menu (ImGuiDebugMenu.cpp)
  └─ The actual UI content. Currently shows the ImGui demo window.
     This is where game-specific debug tools will be added.
```

All code gated behind `#ifdef RTS_HAS_IMGUI` / `RTS_BUILD_OPTION_IMGUI`.


## Contributing

We welcome contributions to the project! If you’re interested in contributing, you need to have knowledge of C++. Join
the developer chat on Discord for more information on how to get started. Please make sure to read our
[Contributing Guidelines](CONTRIBUTING.md) before submitting a pull request. You can also check out 
the [Wiki](https://github.com/TheSuperHackers/GeneralsGameCode/wiki) for more detailed documentation.


## License & Legal Disclaimer

EA has not endorsed and does not support this product. All trademarks are the property of their respective owners.

This project is licensed under the [GPL-3.0 License](https://www.gnu.org/licenses/gpl-3.0.html), which allows you to
freely modify and distribute the source code under the terms of this license. Please see [LICENSE.md](LICENSE.md) 
for details.