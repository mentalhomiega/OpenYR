# The voxel drawers and the walk the GPU voxel renderer draws from are compiled straight into the
# harness. It lives outside code/ so that the recursive glob building OpenTS cannot pick this
# target's entry point up.

opents_add_test(GpuVoxel
    NAME gpuvoxel
    SOURCES gpuvoxeltest.cpp
    ENGINE
        dbgprint.cpp
        voxlib.cpp
        emissive.cpp
        writewatch.cpp
        gpuvoxel.cpp
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR} ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    FLOAT
)
