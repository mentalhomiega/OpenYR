# The rollback link is compiled directly into this asset-free harness and driven through
# sockets that reach no network, with a clock the test sets.

opents_add_test(RollbackLink
    NAME rollbacklink
    SOURCES linkcontract.cpp
    ENGINE
        ipxaddr.cpp
        netsocket_null.cpp
        nettime.cpp
        rollbacklink.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    LIBRARIES winmm ws2_32
)
