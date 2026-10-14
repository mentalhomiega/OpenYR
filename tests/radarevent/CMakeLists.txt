# The radar event table, the suppression rule and the recent-cell ring are inline in revent.hh.
# The suppression distance calls gamemd's 2D length, which gmmath.cpp defines, so that unit is
# compiled in too.

opents_add_test(RadarEvent
    NAME radarevent
    SOURCES radareventcontract.cpp
    ENGINE gmmath.cpp
    DEFINITIONS WIN32 _WINDOWS NOMINMAX
)
