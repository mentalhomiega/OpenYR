# The production rules are header-only; always.h supplies Truncate_To_Single and needs the
# Windows build's definitions. FLOAT keeps the single-precision stores the engine builds with.

opents_add_test(ProductionRules
    NAME productionrules
    SOURCES productionrulestest.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    FLOAT
)
