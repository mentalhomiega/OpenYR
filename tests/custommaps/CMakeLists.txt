# The readers for what map editors write are header-only helpers in code/, so the harness needs
# no engine sources and no game data.

opents_add_test(CustomMaps
    NAME custommaps
    SOURCES custommaps.cpp
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
)
