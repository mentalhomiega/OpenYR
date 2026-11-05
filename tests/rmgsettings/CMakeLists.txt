# The RMGMD.INI reader and the values derived from it. The INI reader is compiled in so the
# settings are read from text, as the game reads the file.

opents_add_test(RMGSettings
    NAME rmgsettings
    SOURCES rmgsettingstest.cpp
    ENGINE
        rmgsettings.cpp
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
    FLOAT
    STAMP
)
