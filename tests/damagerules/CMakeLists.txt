# The prone damage step is header-only, so the harness compiles nothing from code/.

opents_add_test(DamageRules
    NAME damagerules
    SOURCES damagerulestest.cpp
    FLOAT
)
