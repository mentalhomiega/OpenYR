# The audio check tool's tests make up their own recordings, so they need no game files. They need Python 3;
# without it they are not added and the message says so. Without numpy the tool falls back to slower matching,
# and the tests still pass.

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    add_test(NAME audiocheck
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/test_audio_check.py"
    )
else()
    message(STATUS "audiocheck tests need Python 3; not added")
endif()
