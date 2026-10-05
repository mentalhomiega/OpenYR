# The mod folders are compiled straight into the harness, along with the directory handling,
# the file classes and the INI reader they stand on. It lives outside code/ so that the
# recursive glob building OpenTS cannot pick this target's entry point up.

opents_add_test(Mods
    NAME mods
    SOURCES modscontract.cpp
    ENGINE
        mods.cpp
        gamedirs.cpp
        cdfile.cpp
        bfiofile.cpp
        rawfile.cpp
        file.cpp
        file_posix.cpp
        file_win.cpp
        dbgprint.cpp
        ini.cpp
        utf8.cpp
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
    UTF8
    STAMP
)
