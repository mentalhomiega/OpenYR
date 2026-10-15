# The free player colors are compiled straight into the harness: the packing, the remap ramp
# step, the CVD-aware distance and the presets. It lives outside code/ so that the recursive
# glob building the engine cannot pick this target's entry point up.

opents_add_test(PlayerColor
    NAME playercolor
    SOURCES playercolortest.cpp
    ENGINE
        playercolor.cpp
        hsv.cpp
        rgb.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    FLOAT
)
