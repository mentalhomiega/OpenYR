# The superweapon key parsing is header-only and needs no engine code.

opents_add_test(SuperRules
    NAME swrules
    SOURCES swrulestest.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
