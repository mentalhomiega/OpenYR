# The shadow once marks and the blit paths that use them are compiled straight into the harness. It lives
# outside code/ so that the recursive glob building OpenTS cannot pick this target's entry point up.

opents_add_test(ShadowOnce
    NAME shadowonce
    SOURCES shadowoncetest.cpp
    ENGINE
        shadowonce.cpp
        _surface.cpp
        emissive.cpp
        emissive_surface.cpp
        blit.cpp
        buff.cpp
        xsurface.cpp
        zbuffer.cpp
        abuffer.cpp
        ringbuffer.cpp
        _zbuffer.cpp
        writewatch.cpp
        tilediff.cpp
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
