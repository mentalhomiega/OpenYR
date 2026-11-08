# The [Lighting] conversions are header-only; the INI reader is compiled in so a written
# scenario can be read back as the game reads it.

opents_add_test(LightingIni
    NAME lightingini
    SOURCES lightinginitest.cpp
    ENGINE
        ini.cpp
        utf8.cpp
        dbgprint.cpp
        b64pipe.cpp
        b64straw.cpp
        base64.cpp
        buff.cpp
        crc.cpp
        cstraw.cpp
        int.cpp
        mpmath.cpp
        pipe.cpp
        pk.cpp
        straw.cpp
        trim.cpp
        xpipe.cpp
        xstraw.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    STAMP
)
