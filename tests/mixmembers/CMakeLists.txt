# The archive reader is compiled straight into the harness. It lives outside code/ so that the
# recursive glob building the engine cannot pick this target's entry point up.

if(MSVC)
    set(MIXMEMBERS_FILE_PLATFORM file_win.cpp)
else()
    set(MIXMEMBERS_FILE_PLATFORM file_posix.cpp)
endif()

opents_add_test(MixMembersContract
    NAME mixmembers
    SOURCES mixmemberscontract.cpp
    ENGINE
        mixfile.cpp ccfile.cpp cdfile.cpp rawfile.cpp startuptrace.cpp file.cpp ${MIXMEMBERS_FILE_PLATFORM} bfiofile.cpp
        pk.cpp pkstraw.cpp shastraw.cpp sha.cpp blwstraw.cpp blowfish.cpp rndstraw.cpp random.cpp _syncrec.cpp syncrec.cpp
        buff.cpp crc.cpp int.cpp mpmath.cpp pipe.cpp straw.cpp xpipe.cpp xstraw.cpp dbgprint.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    STAMP
)
