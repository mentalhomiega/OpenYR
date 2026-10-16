# The rules are header-only, so the harness compiles nothing from code/. FLOAT keeps the
# single-precision store of the experience total.

opents_add_test(VeteranRules
    NAME veteranrules
    SOURCES veteranrulestest.cpp
    FLOAT
)
