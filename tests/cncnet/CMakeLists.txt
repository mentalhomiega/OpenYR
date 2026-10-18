opents_add_test(CnCNetTest
    NAME cncnet
    SOURCES cncnettest.cpp
    ENGINE
        cncnet/ircmessage.cpp
        cncnet/ircclient.cpp
        cncnet/cncnetgame.cpp
)
