# The voxel library is compiled straight into the harness. It lives outside code/ so that the
# recursive glob building OpenTS cannot pick this target's entry point up.

opents_add_test(VoxelDraw
    NAME voxeldraw
    SOURCES voxeldraw.cpp
    ENGINE
        dbgprint.cpp
        voxlib.cpp
        emissive.cpp
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    FLOAT
)
