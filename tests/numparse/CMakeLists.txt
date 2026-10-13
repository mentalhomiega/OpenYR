# The parser is header-only, so the harness compiles just its own source. FLOAT matches the
# engine's floating-point flags, so the rounding it checks is the engine's.

opents_add_test(NumParseContract
    NAME numparse
    SOURCES numparse_test.cpp
    FLOAT
)
