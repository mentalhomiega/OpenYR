# The setup's logic is compiled into a static library from tools/setup and driven here against
# temporary folders. The test never reads a real Yuri's Revenge install or touches the player's
# desktop, Start Menu or Apps & Features entries: its shortcut folders and registry key are its own.

opents_add_test(SetupTests
    NAME setup
    SOURCES setup_tests.cpp
    LIBRARIES OpenYRSetupCore advapi32 ole32 uuid
)
