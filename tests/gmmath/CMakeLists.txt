# The helpers are compiled straight into the harness from code/. It lives outside code/ so that
# the recursive glob building OpenTS cannot pick this target's entry point up.
#
# FLOAT carries the float semantics the engine builds with: the helpers round their intermediate
# products toward zero, and the harness has to see the same single-precision stores.

opents_add_test(GmMath
    NAME gmmath
    SOURCES gmmathtest.cpp
    ENGINE gmmath.cpp
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    FLOAT
)
