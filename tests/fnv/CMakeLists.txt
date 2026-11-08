# The hashes are compiled straight into the harness with the code that owns them. It lives
# outside code/ so that the recursive glob building OpenTS cannot pick this target's entry
# point up.

opents_add_test(FnvTest
    NAME fnv
    SOURCES fnvtest.cpp
    ENGINE
        audio/audiosample.cpp
        audio/audiodecode.cpp
        soscodec.cpp
        cncnetcopy.cpp
        cncnet/cncnettext.cpp
        modsync.cpp
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    LIBRARIES miniaudio advapi32
    FLOAT
    STAMP
)
