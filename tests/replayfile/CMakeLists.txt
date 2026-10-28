# The replay file reader and writer are compiled straight into the harness. They live in code/
# but have no engine dependencies, so the harness compiles them alone. The entry point stays here
# so the recursive glob building OpenTS cannot pick it up.

opents_add_test(ReplayFile
    NAME replayfile
    SOURCES replayfilecontract.cpp
    ENGINE
        replayfile.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
