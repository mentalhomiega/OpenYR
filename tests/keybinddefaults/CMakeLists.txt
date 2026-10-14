# The keyboard defaults and their upgrade are plain data and functions, so the harness
# compiles only that unit.

opents_add_test(KeybindDefaults
    NAME keybinddefaults
    SOURCES keybinddefaultscontract.cpp
    ENGINE
        keybinddefaults.cpp
    DEFINITIONS WIN32 _WINDOWS NOMINMAX
)
