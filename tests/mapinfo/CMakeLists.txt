# The map lister's tests run on a hand-written sample, so they need no game files. They need Python 3;
# without it they are not added and the message says so.

find_package(Python3 COMPONENTS Interpreter QUIET)
if(Python3_Interpreter_FOUND)
    add_test(NAME mapinfo-sample
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_CURRENT_SOURCE_DIR}/check_mapinfo.py" $<TARGET_FILE:mapinfo> "${CMAKE_CURRENT_SOURCE_DIR}/sample.map"
    )
    add_test(NAME mapinfo-tables-current
        COMMAND "${Python3_EXECUTABLE}" "${CMAKE_SOURCE_DIR}/tools/mapinfo/gen_tables.py" --check
    )
else()
    message(STATUS "mapinfo tests need Python 3; not added")
endif()
