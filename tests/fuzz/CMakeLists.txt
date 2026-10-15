# Deterministic fuzz harnesses for the parsers that read data from other players. Each harness
# is its own executable, built from fuzz.cpp and the engine sources of one parser, and
# registers one CTest test named fuzz_<parser>.
#
# Two ways to build them:
#
#   Inside the project (Visual Studio), as part of tests/. This is the supported build.
#
#   Standalone on Linux, where the project itself refuses to configure. Clang compiles the
#   engine sources against the stand-in Windows headers in shim/, with the sanitizers:
#
#     cmake -S tests/fuzz -B build-fuzz -G Ninja -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang
#     cmake --build build-fuzz -j 2
#     ctest --test-dir build-fuzz
#
#   The standalone build links with the system C++ driver, so it works where clang's own
#   sanitizer runtime is missing but GCC's libasan and libubsan are installed. Set
#   OPENTS_FUZZ_SANITIZE=OFF to build without them.

if(COMMAND opents_add_test)
    set(FUZZ_STANDALONE OFF)
else()
    set(FUZZ_STANDALONE ON)
endif()

if(FUZZ_STANDALONE)
    cmake_minimum_required(VERSION 3.23)
    project(OpenTSFuzz C CXX)
    enable_testing()

    set(FUZZ_REPO "${CMAKE_CURRENT_SOURCE_DIR}/../..")
    get_filename_component(FUZZ_REPO "${FUZZ_REPO}" ABSOLUTE)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/test-bin")

    option(OPENTS_FUZZ_SANITIZE "Build the harnesses with AddressSanitizer and UndefinedBehaviorSanitizer" ON)
    option(OPENTS_FUZZ_SYSTEM_LINKER_DRIVER "Link with the system C++ compiler so GCC's sanitizer runtime is used" ON)

    set(FUZZ_SANITIZER_FLAGS "")
    if(OPENTS_FUZZ_SANITIZE)
        # The vptr check is off because the inherited intrusive lists hand out their head node as
        # an element of the derived type, which that check reports on every walk of a list.
        set(FUZZ_SANITIZER_FLAGS -fsanitize=address,undefined -fno-sanitize=vptr -fno-sanitize-recover=undefined -fno-omit-frame-pointer)
    endif()

    if(OPENTS_FUZZ_SYSTEM_LINKER_DRIVER)
        # CMake links with the compiler it compiled with. clang without compiler-rt cannot link
        # the sanitizers; the system g++ finds libasan and libubsan.
        find_program(FUZZ_LINKER_DRIVER NAMES g++ c++)
        if(FUZZ_LINKER_DRIVER)
            set(CMAKE_CXX_LINK_EXECUTABLE "${FUZZ_LINKER_DRIVER} <FLAGS> <CMAKE_CXX_LINK_FLAGS> <LINK_FLAGS> <OBJECTS> -o <TARGET> <LINK_LIBRARIES>")
        endif()
    endif()

    # The vendored LZO library, built as the engine builds it.
    file(GLOB FUZZ_LZO_SOURCES CONFIGURE_DEPENDS "${FUZZ_REPO}/thirdparty/lzo/src/*.c")
    add_library(lzo STATIC ${FUZZ_LZO_SOURCES})
    target_include_directories(lzo PUBLIC "${FUZZ_REPO}/thirdparty/lzo/include" PRIVATE "${FUZZ_REPO}/thirdparty/lzo/src")
    target_compile_options(lzo PRIVATE -w ${FUZZ_SANITIZER_FLAGS} -g -O1)
else()
    set(FUZZ_REPO "${CMAKE_SOURCE_DIR}")
endif()


# Builds one harness and registers its test. ENGINE names engine sources relative to code/;
# SOURCES are harness sources relative to this directory.
function(fuzz_add_target target)
    cmake_parse_arguments(FUZZ "" "NAME" "SOURCES;ENGINE;LIBRARIES" ${ARGN})

    set(sources "${CMAKE_CURRENT_SOURCE_DIR}/fuzz.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/fuzz_debug.cpp")
    foreach(source IN LISTS FUZZ_SOURCES)
        list(APPEND sources "${CMAKE_CURRENT_SOURCE_DIR}/${source}")
    endforeach()
    set(engine_sources "")
    foreach(source IN LISTS FUZZ_ENGINE)
        list(APPEND engine_sources "${FUZZ_REPO}/code/${source}")
    endforeach()

    if(FUZZ_STANDALONE)
        add_executable(${target} ${sources} ${engine_sources})
        target_include_directories(${target} PRIVATE
            "${FUZZ_REPO}/code" "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/shim")
        target_compile_definitions(${target} PRIVATE NOMINMAX)
        target_compile_options(${target} PRIVATE
            -fms-extensions -include "${CMAKE_CURRENT_SOURCE_DIR}/shim/compat.h"
            ${FUZZ_SANITIZER_FLAGS} -g -O1)
        # The engine sources are inherited and warn heavily; the harness sources are not.
        set_source_files_properties(${engine_sources} PROPERTIES COMPILE_OPTIONS "-w")
        target_link_options(${target} PRIVATE ${FUZZ_SANITIZER_FLAGS})
        target_link_libraries(${target} PRIVATE ${FUZZ_LIBRARIES} pthread)
        add_test(NAME ${FUZZ_NAME} COMMAND ${target})
    else()
        opents_add_test(${target}
            NAME ${FUZZ_NAME}
            SOURCES ${FUZZ_SOURCES} fuzz.cpp fuzz_debug.cpp
            ENGINE ${FUZZ_ENGINE}
            DEFINITIONS WIN32 _WINDOWS _MBCS NOMINMAX
            LIBRARIES ${FUZZ_LIBRARIES}
        )
        target_include_directories(${target} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    set_target_properties(${target} PROPERTIES OUTPUT_NAME ${FUZZ_NAME})
    set_tests_properties(${FUZZ_NAME} PROPERTIES TIMEOUT 300)
endfunction()


# The directory search behind the file classes has one source per platform.
if(MSVC)
    set(FUZZ_FILE_PLATFORM file_win.cpp)
else()
    set(FUZZ_FILE_PLATFORM file_posix.cpp)
endif()

# The INI reader and the sources it is built from.
set(FUZZ_INI_ENGINE
    ini.cpp
    utf8.cpp
    b64pipe.cpp
    b64straw.cpp
    base64.cpp
    buff.cpp
    crc.cpp
    cstraw.cpp
    int.cpp
    mpmath.cpp
    pipe.cpp
    pk.cpp
    straw.cpp
    trim.cpp
    xpipe.cpp
    xstraw.cpp
)

fuzz_add_target(FuzzIni NAME fuzz_ini
    SOURCES fuzz_ini.cpp
    ENGINE ${FUZZ_INI_ENGINE}
)

fuzz_add_target(FuzzSpawn NAME fuzz_spawn
    SOURCES fuzz_spawn.cpp
    ENGINE ${FUZZ_INI_ENGINE} spawnerconfig.cpp
)

fuzz_add_target(FuzzLcw NAME fuzz_lcw
    SOURCES fuzz_lcw.cpp
    ENGINE ${FUZZ_INI_ENGINE} lcw.cpp lcwpipe.cpp lcwstraw.cpp
)

fuzz_add_target(FuzzLzo NAME fuzz_lzo
    SOURCES fuzz_lzo.cpp
    ENGINE ${FUZZ_INI_ENGINE} lzopipe.cpp lzostraw.cpp
    LIBRARIES lzo
)

fuzz_add_target(FuzzNet NAME fuzz_net
    SOURCES fuzz_net.cpp
    ENGINE netglobal.cpp mpload.cpp
)

fuzz_add_target(FuzzCsf NAME fuzz_csf
    SOURCES fuzz_csf.cpp
    ENGINE csf.cpp utf8.cpp buff.cpp straw.cpp xstraw.cpp
)

fuzz_add_target(FuzzMix NAME fuzz_mix
    SOURCES fuzz_mix.cpp
    ENGINE mixfile.cpp ccfile.cpp cdfile.cpp rawfile.cpp file.cpp ${FUZZ_FILE_PLATFORM} bfiofile.cpp pk.cpp pkstraw.cpp
        shastraw.cpp sha.cpp blwstraw.cpp blowfish.cpp rndstraw.cpp random.cpp _syncrec.cpp syncrec.cpp
        buff.cpp crc.cpp int.cpp mpmath.cpp pipe.cpp straw.cpp xpipe.cpp xstraw.cpp
)

fuzz_add_target(FuzzImage NAME fuzz_image
    SOURCES fuzz_image.cpp
    ENGINE ui/rml/rmlimage.cpp
)

fuzz_add_target(FuzzVoxel NAME fuzz_voxel
    SOURCES fuzz_voxel.cpp
    ENGINE voxlib.cpp motlib.cpp voxpal.cpp matrix3d.cpp gmmath.cpp
)
