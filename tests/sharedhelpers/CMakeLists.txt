# The header-only helpers that several parts of the engine share (little-endian writers, trimming
# and the other text helpers) are compiled straight into the harness so their edge cases stay
# pinned. It lives outside code/ so that the recursive glob building OpenTS cannot pick this
# target's entry point up.

opents_add_test(SharedHelpers
    NAME sharedhelpers
    SOURCES sharedhelperstest.cpp
    ENGINE
        ahtoi.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
