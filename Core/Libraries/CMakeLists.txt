# WW common libraries decended from earliest C&C games
add_subdirectory(Source/WWVegas)

# profiling library
add_subdirectory(Source/profile)

# debugging library
add_subdirectory(Source/debug)

add_subdirectory(Source/EABrowserDispatch)
add_subdirectory(Source/EABrowserEngine)
add_subdirectory(Source/Compression)

# TheSuperHackers @feature zhp47 15/04/2026 Add ImGui for in-game debug overlay
if(RTS_BUILD_OPTION_IMGUI)
    add_subdirectory(Source/ImGui)
else()
    add_library(lib_imgui INTERFACE)
endif()
