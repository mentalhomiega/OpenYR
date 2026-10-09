# The harness compiles _mission.cpp for the name table and reads it through the header-only
# rules in missionrules.h.

opents_add_test(MissionNames
    NAME missionnames
    SOURCES missionnamestest.cpp
    ENGINE _mission.cpp
)
