# The binding model, the keyboard file it reads and writes, and the trigger timing are plain data and
# functions, so the harness compiles only those units.

opents_add_test(Keybindings
    NAME keybindings
    SOURCES keybindingscontract.cpp
    ENGINE
        keybinddefaults.cpp
        keybindings.cpp
        hotkeytiming.cpp
    DEFINITIONS WIN32 _WINDOWS NOMINMAX
)
