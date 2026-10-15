# The grid build keys map letters onto the visible sidebar cameos, which is plain data and functions, so
# the harness compiles only that unit.

opents_add_test(SidebarGrid
    NAME sidebargrid
    SOURCES sidebargridcontract.cpp
    ENGINE
        sidebargrid.cpp
    DEFINITIONS WIN32 _WINDOWS NOMINMAX
)
