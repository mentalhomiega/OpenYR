# The warhead rules are header-only, so the harness compiles nothing from code/.

opents_add_test(WarheadRules
    NAME warheadrules
    SOURCES warheadrulestest.cpp
    FLOAT
)
