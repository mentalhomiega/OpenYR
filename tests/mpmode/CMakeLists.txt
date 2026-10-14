# The mode table's reader is compiled straight into the harness along with the INI reader it
# is written against. It lives outside code/ so that the recursive glob building the engine
# cannot pick this target's entry point up.

opents_add_test(MPModeContract
    NAME mpmodecontract
    SOURCES mpmodecontract.cpp
    ENGINE
        mpmodeparse.cpp
        ini.cpp
        utf8.cpp
        dbgprint.cpp
        trim.cpp
        buff.cpp
        straw.cpp
        xstraw.cpp
        cstraw.cpp
        pipe.cpp
        xpipe.cpp
        b64straw.cpp
        b64pipe.cpp
        base64.cpp
        crc.cpp
        pk.cpp
        int.cpp
        mpmath.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    UTF8
    STAMP
)
