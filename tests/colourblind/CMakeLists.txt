# The colour-blind matrices are plain arithmetic, so the harness compiles only colourblind.cpp. FLOAT matches
# the engine's floating-point flags, so the rounding it checks is the engine's.

opents_add_test(ColourBlindContract
    NAME colourblind
    SOURCES colourblind_test.cpp
    ENGINE colourblind.cpp
    FLOAT
)
