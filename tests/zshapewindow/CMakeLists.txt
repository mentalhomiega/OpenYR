# The window helpers are header-only, so the harness compiles just its own source. The sizes are those
# of BUILDNGZ.SHP, but the test builds its own shape in memory, so it needs no game data.

opents_add_test(ZShapeWindow
    NAME zshapewindow
    SOURCES zshapewindowtest.cpp
)
