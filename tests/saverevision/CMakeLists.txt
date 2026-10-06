# The save listing block is compiled straight into the harness with the file it is written to,
# so the revision a save carries is checked from SaveVersionInfo through the field table and
# back. It lives outside code/ so that the recursive glob building OpenTS cannot pick this
# target's entry point up.

opents_add_test(SaveRevision
    NAME saverevision
    SOURCES saverevisioncontract.cpp
    ENGINE
        crc.cpp
        savefile.cpp
        savever.cpp
        utf8.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    LIBRARIES lzo
    STAMP
)
