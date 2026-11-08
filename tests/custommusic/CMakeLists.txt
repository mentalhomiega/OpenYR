# The folder scan and the playlist rules are plain C++ with no engine state, so the harness
# compiles only custommusic.cpp and needs no game data or audio device.

opents_add_test(CustomMusic
    NAME custommusic
    SOURCES custommusictest.cpp
    ENGINE
        custommusic.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
