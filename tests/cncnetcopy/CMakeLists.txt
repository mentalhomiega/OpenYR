# The tester copy's logic is compiled straight into the harness. It lives outside code/ so that
# the recursive glob building the engine cannot pick this target's entry point up.
#
# The launch stub is a separate program that writes launched.txt beside itself when it runs. The
# harness copies it over the copy's launcher to check the launch without starting any CnCNet file.

add_executable(cncnetdummylauncher dummylauncher.cpp)
set_target_properties(cncnetdummylauncher PROPERTIES FOLDER "Tests")

opents_add_test(CNCNetCopyTest
    NAME cncnetcopy
    SOURCES cncnetcopytest.cpp
    ENGINE
        cncnetcopy.cpp
    INCLUDES ${OPENTS_GENERATED_DIR}
    DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
    LIBRARIES advapi32
)

add_dependencies(CNCNetCopyTest cncnetdummylauncher)
