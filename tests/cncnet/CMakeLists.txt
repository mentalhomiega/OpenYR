opents_add_test(CnCNetTest
    NAME cncnet
    SOURCES cncnettest.cpp
    ENGINE
        cncnet/ircmessage.cpp
        cncnet/ircclient.cpp
        cncnet/cncnetgame.cpp
        cncnet/cncnetlobby.cpp
        cncnet/cncnetroom.cpp
        cncnet/cncnetsha1.cpp
        cncnet/cncnetspawn.cpp
        cncnet/cncnettunnel.cpp
        cncnet/cncnettunnellist.cpp
)
if(WIN32)
    target_link_libraries(CnCNetTest PRIVATE ws2_32)
endif()

# A manual probe, not run by CTest: connects a lobby session to an IRC server (normally the
# stand-in in scripts/cncnet_irc_standin.py) and prints what the lobby sees.
add_executable(CnCNetProbe
    cncnetprobe.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/ircmessage.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/ircclient.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/cncnetgame.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/cncnetlobby.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/cncnetsession.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/tcpstream.cpp
)
target_compile_features(CnCNetProbe PRIVATE cxx_std_20)
target_include_directories(CnCNetProbe PRIVATE "${CMAKE_SOURCE_DIR}/code")
set_target_properties(CnCNetProbe PROPERTIES FOLDER "Tests")
target_compile_options(CnCNetProbe PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc> $<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Release>:/MT>>)
if(WIN32)
    target_link_libraries(CnCNetProbe PRIVATE ws2_32)
endif()

# A manual probe, not run by CTest: downloads the live tunnel master list and pings the tunnels.
add_executable(CnCNetTunnelProbe
    cncnettunnelprobe.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/cncnethttp.cpp
    ${CMAKE_SOURCE_DIR}/code/cncnet/cncnettunnellist.cpp
)
target_compile_features(CnCNetTunnelProbe PRIVATE cxx_std_20)
target_include_directories(CnCNetTunnelProbe PRIVATE "${CMAKE_SOURCE_DIR}/code")
set_target_properties(CnCNetTunnelProbe PROPERTIES FOLDER "Tests")
target_compile_options(CnCNetTunnelProbe PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/EHsc> $<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Release>:/MT>>)
if(WIN32)
    target_link_libraries(CnCNetTunnelProbe PRIVATE ws2_32 winhttp)
endif()
