# The mod signature is compiled straight into this asset-free harness, which writes its own
# mod folders. It lives outside code/ so that the recursive glob building OpenTS cannot pick
# this target's entry point up.

opents_add_test(ModSyncContract
    NAME modsync
    SOURCES modsynccontract.cpp
    ENGINE
        modsync.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    STAMP
)
