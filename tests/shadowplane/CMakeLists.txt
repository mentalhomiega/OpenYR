# The shadow plane and the blit paths that mark it are compiled straight into the harness. It lives
# outside code/ so that the recursive glob building OpenTS cannot pick this target's entry point up.

opents_add_test(ShadowPlane
    NAME shadowplane
    SOURCES shadowplanetest.cpp
    ENGINE
        shadowplane.cpp
        _surface.cpp
        emissive.cpp
        emissive_surface.cpp
        blit.cpp
        buff.cpp
        xsurface.cpp
        zbuffer.cpp
        abuffer.cpp
        _zbuffer.cpp
        writewatch.cpp
    INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
