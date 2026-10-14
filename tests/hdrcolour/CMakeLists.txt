# The colour maths is plain arithmetic, so the harness compiles only hdrcolour.cpp. FLOAT matches
# the engine's floating-point flags, so the rounding it checks is the engine's.

opents_add_test(HDRColourContract
    NAME hdrcolour
    SOURCES hdrcolour_test.cpp
    ENGINE hdrcolour.cpp
    FLOAT
)
