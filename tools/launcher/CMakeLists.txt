# OpenYR.exe, the launcher that starts Game.exe and offers the beta updates. Its logic is a static
# library, so tests/launcher drives the feed, the decision and the INI code without the network.
# The window, WinHTTP and the process code are in the executable.

if(NOT WIN32)
    return()
endif()

add_library(OpenYRLauncherCore STATIC
    launcher_core.cpp
)
target_include_directories(OpenYRLauncherCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(OpenYRLauncherCore PUBLIC cxx_std_20)
target_link_libraries(OpenYRLauncherCore PUBLIC bcrypt advapi32)
target_compile_definitions(OpenYRLauncherCore PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(OpenYRLauncherCore PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHsc /W4>)
set_property(TARGET OpenYRLauncherCore PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_target_properties(OpenYRLauncherCore PROPERTIES FOLDER "Tools")

# OpenYR.exe: the launcher's window, WinHTTP, BCrypt and the process code, on top of the core library.
add_executable(OpenYRLauncher WIN32
    launcher_main.cpp
    launcher_system.cpp
    launcher.rc
)
target_compile_features(OpenYRLauncher PRIVATE cxx_std_20)
target_compile_definitions(OpenYRLauncher PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(OpenYRLauncher PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHsc /W4>)
target_link_libraries(OpenYRLauncher PRIVATE OpenYRLauncherCore comctl32 winhttp bcrypt user32 gdi32 shell32 advapi32)
set_property(TARGET OpenYRLauncher PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_target_properties(OpenYRLauncher PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "OpenYR"
    FOLDER "Tools"
)
target_link_options(OpenYRLauncher PRIVATE /MANIFEST:NO)
