# The table is header-only, so the harness compiles nothing from code/. FLOAT keeps the
# single-precision sum that sizes a blast from CellSpread.

opents_add_test(CellSpread
    NAME cellspread
    SOURCES cellspreadtest.cpp
    FLOAT
)
