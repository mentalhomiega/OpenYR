# The WAV writer is compiled straight into the harness. It lives outside code/ so that the
# recursive glob building OpenTS cannot pick this target's entry point up.

opents_add_test(AudioWav
    NAME audiowav
    SOURCES audiowav.cpp
    ENGINE
        audio/audiowav.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS
    FLOAT
)
