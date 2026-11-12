# The performance timing and its history are plain data and the steady clock, so the harness
# compiles those units with a stand-in for the debug log.

opents_add_test(PerfStat
    NAME perfstat
    SOURCES
        perfstatcontract.cpp
        perfstatstubs.cpp
    ENGINE
        perfring.cpp
        perfstat.cpp
    DEFINITIONS WIN32 _WINDOWS NOMINMAX
)
