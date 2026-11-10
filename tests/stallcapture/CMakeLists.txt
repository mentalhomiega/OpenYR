# The capture code is compiled straight into the harness. It needs no game data.

opents_add_test(StallCaptureTest
    NAME stallcapture
    SOURCES stallcapturetest.cpp
    ENGINE
        stallcapture.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
