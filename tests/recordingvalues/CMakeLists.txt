# The recording values module is compiled straight into the harness; it needs only the header-only
# flag serializer of SpecialClass from the engine.

opents_add_test(RecordingValues
    NAME recordingvalues
    SOURCES recordingvaluestest.cpp
    ENGINE
        recordingvalues.cpp
)
