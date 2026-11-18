# The state is fed SDL key events built by hand and the test's own list of the keys Windows
# holds, so it needs no window and no keyboard.

opents_add_test(SDLInputContract
    NAME sdlinputcontract
    SOURCES sdlinputcontract.cpp
    ENGINE
        sdl/sdlinput.cpp
        sdl/sdlkeys.cpp
    DEFINITIONS WIN32 _WINDOWS NOMINMAX
    LIBRARIES SDL3::SDL3-static
)
