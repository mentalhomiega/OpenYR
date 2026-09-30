# The Unicode font reader is compiled straight into the harness. It lives outside code/ so that
# the recursive glob building the engine cannot pick this target's entry point up.

opents_add_test(BitFontContract
    NAME bitfontcontract
    SOURCES bitfontcontract.cpp
    ENGINE
        bitfontdata.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    UTF8
    STAMP
)
