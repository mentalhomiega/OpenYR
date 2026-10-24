# miniz comes from bimg, as in the engine.

opents_add_test(ReportTest
    NAME report
    SOURCES reporttest.cpp
    ENGINE
        report/reportupload.cpp
    INCLUDES
        ${CMAKE_SOURCE_DIR}/thirdparty/bgfx.cmake/bimg/3rdparty/tinyexr/deps
    LIBRARIES bimg
)
