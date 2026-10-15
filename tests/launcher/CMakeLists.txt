# The launcher's feed, version, INI and quoting logic, driven without the network or a window, and an
# end-to-end run of OpenYR.exe and its update copy against a local feed and fake packages. The
# end-to-end script runs the launcher with /QUIET and /NOGAME and the setup with /NOSHORTCUTS, so it
# shows no dialog, starts no game and writes nothing outside its temporary folder.

if(NOT WIN32)
    return()
endif()

opents_add_test(LauncherTests
    NAME launcher
    SOURCES launcher_tests.cpp
    INCLUDES "${CMAKE_SOURCE_DIR}/tools/common"
    LIBRARIES OpenYRLauncherCore advapi32 shell32
)

add_dependencies(LauncherTests OpenYRLauncher OpenYRSetup)

add_test(NAME launcher_e2e
    COMMAND powershell.exe -NoProfile -ExecutionPolicy Bypass
        -File "${CMAKE_CURRENT_SOURCE_DIR}/launcher_e2e.ps1"
        -Launcher "$<TARGET_FILE:OpenYRLauncher>"
        -Setup "$<TARGET_FILE:OpenYRSetup>"
)
