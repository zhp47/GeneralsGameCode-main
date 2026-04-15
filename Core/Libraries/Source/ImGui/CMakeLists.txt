# ImGui integration for in-game debug UI.
# Currently depends on Win32 + DirectX 8. When disabled or on unsupported
# platforms, lib_imgui is an empty INTERFACE library so that consumers can
# link unconditionally without #ifdefs in CMake.

if (NOT WIN32)
    add_library(lib_imgui INTERFACE)
    return()
endif()

FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG 791ad9b82db44ada9fedb3e26b2d900974ac0959
    SYSTEM
)

FetchContent_MakeAvailable(imgui)

set(IMGUI_BASE_SRCS
    "${imgui_SOURCE_DIR}/imgui.cpp"
    "${imgui_SOURCE_DIR}/imgui_draw.cpp"
    "${imgui_SOURCE_DIR}/imgui_tables.cpp"
    "${imgui_SOURCE_DIR}/imgui_widgets.cpp"
    "${imgui_SOURCE_DIR}/imgui_demo.cpp"
)

set(IMGUI_PLATFORM_SRCS
    "${imgui_SOURCE_DIR}/backends/imgui_impl_win32.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/dx8_backend/imgui_impl_dx8.cpp"
)

set(IMGUI_WRAPPER_SRCS
    "${CMAKE_CURRENT_LIST_DIR}/wrapper/ImGuiFrameManager.cpp"
    "${CMAKE_CURRENT_LIST_DIR}/wrapper/ImGuiContextManager.cpp"
)

set(IMGUI_INCLUDE_DIRS
    "${imgui_SOURCE_DIR}"
    "${imgui_SOURCE_DIR}/backends"
    "${CMAKE_CURRENT_LIST_DIR}/dx8_backend"
)

message(STATUS "Enabling ImGui (Win32 + DX8)")

add_library(lib_imgui STATIC
    ${IMGUI_BASE_SRCS}
    ${IMGUI_PLATFORM_SRCS}
    ${IMGUI_WRAPPER_SRCS}
)

target_include_directories(lib_imgui
    PUBLIC ${IMGUI_INCLUDE_DIRS}
    PUBLIC "${CMAKE_CURRENT_LIST_DIR}/wrapper"
)

target_link_libraries(lib_imgui PRIVATE d3d8lib)

target_compile_definitions(lib_imgui
    PRIVATE IMGUI_DISABLE_DEFAULT_IMCONFIG
    PRIVATE IMGUI_USER_CONFIG="${CMAKE_CURRENT_LIST_DIR}/imconfig.h"
    INTERFACE RTS_HAS_IMGUI
)
