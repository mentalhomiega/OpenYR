# The event length table and the packet decoder are compiled straight into the harness, so the
# payload each event carries in a packet, a replay and a sync record can be pinned without starting
# the engine. The entry point stays here so the recursive glob building OpenTS cannot pick it up.

opents_add_test(EventPayload
    NAME eventpayload
    SOURCES eventpayloadcontract.cpp
    ENGINE
        _event.cpp
        mpload.cpp
        netadmit.cpp
        netglobal.cpp
        netpacket.cpp
        netreader.cpp
        netsemantic.cpp
        playercolor.cpp
        hsv.cpp
        rgb.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    STAMP
)
