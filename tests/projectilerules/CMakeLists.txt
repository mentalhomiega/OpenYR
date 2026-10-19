# The projectile rules are header-only, so the harness compiles nothing from code/.

opents_add_test(ProjectileRules
    NAME projectilerules
    SOURCES projectilerulestest.cpp
    FLOAT
)
