# The terrain record reader is compiled straight into the harness with the LZO classes it reads
# through. It needs no game data.

opents_add_test(CellRecords
    NAME cellrecords
    SOURCES cellrecords.cpp
    ENGINE
        buff.cpp
        lzopipe.cpp
        lzostraw.cpp
        pipe.cpp
        straw.cpp
        xpipe.cpp
        xstraw.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    LIBRARIES lzo
    FLOAT
)
