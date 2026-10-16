# OpenYR-Setup.exe: one executable that installs OpenYR beside an existing Yuri's Revenge install.
#
# The logic is a static library, so the tests in tests/setup drive the same install, update and
# uninstall code against temporary folders. The window and the silent switches are in setup_main.cpp
# and setup_wizard.cpp. The package is not built in: the packager appends it to the executable.

if(NOT WIN32)
    return()
endif()

add_library(OpenYRSetupCore STATIC
    setup_core.cpp
    setup_find.cpp
    setup_payload.cpp
    setup_system.cpp
)
target_include_directories(OpenYRSetupCore PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_features(OpenYRSetupCore PUBLIC cxx_std_20)
target_compile_definitions(OpenYRSetupCore PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(OpenYRSetupCore PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHsc /W4>)
set_property(TARGET OpenYRSetupCore PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_target_properties(OpenYRSetupCore PROPERTIES FOLDER "Tools")

add_executable(OpenYRSetup WIN32
    setup_main.cpp
    setup_wizard.cpp
    setup.rc
)
target_compile_features(OpenYRSetup PRIVATE cxx_std_20)
target_compile_definitions(OpenYRSetup PRIVATE UNICODE _UNICODE NOMINMAX WIN32_LEAN_AND_MEAN)
target_compile_options(OpenYRSetup PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8 /EHsc /W4>)
target_link_libraries(OpenYRSetup PRIVATE OpenYRSetupCore comctl32 gdi32 user32 shell32 ole32 advapi32 uuid windowscodecs)
set_property(TARGET OpenYRSetup PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
set_target_properties(OpenYRSetup PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
    OUTPUT_NAME "OpenYR-Setup"
    FOLDER "Tools"
)
target_include_directories(OpenYRSetup PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/../common")
# LOAD_LIBRARY_SEARCH_SYSTEM32 for the DLLs Windows loads before WinMain; see tools/common/dll_search.h.
target_link_options(OpenYRSetup PRIVATE /MANIFEST:NO $<$<CXX_COMPILER_ID:MSVC>:/DEPENDENTLOADFLAG:0x800>)
