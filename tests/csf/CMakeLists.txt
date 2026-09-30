# The string table reader is compiled straight into the harness. It lives outside code/ so that
# the recursive glob building the engine cannot pick this target's entry point up.

opents_add_test(CsfContract
    NAME csfcontract
    SOURCES csfcontract.cpp
    ENGINE
        buff.cpp
        csf.cpp
        dbgprint.cpp
        straw.cpp
        xstraw.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    UTF8
    STAMP
)
