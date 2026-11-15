# The settings registry and the rows it builds, against a small declared table and the real
# RA2MD.INI table. The INI reader is compiled in; OptionsClass itself is not, so the real table
# is exercised through a blank instance whose declared members are the only ones touched.

opents_add_test(SettingRegistry
    NAME settingregistry
    SOURCES settingregistrytest.cpp
    ENGINE
        ini.cpp
        startuptrace.cpp
        optionsettings.cpp
        ui/uisettingrows.cpp
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
    UTF8
    STAMP
)
