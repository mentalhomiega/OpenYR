# The launcher's feed, version, INI and quoting logic, driven without the network or a window.

opents_add_test(LauncherTests
    NAME launcher
    SOURCES launcher_tests.cpp
    LIBRARIES OpenYRLauncherCore advapi32 shell32
)
