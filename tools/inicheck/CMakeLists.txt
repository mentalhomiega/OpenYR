# The command-line INI checker. It lives outside code/ so that the recursive glob building the
# engine cannot pick its entry point up, and builds into tools-bin/ rather than bin/, which holds
# what the game runs.

add_executable(inicheck
    main.cpp
    "${CMAKE_SOURCE_DIR}/code/inicheck.cpp"
)
target_compile_features(inicheck PRIVATE cxx_std_20)
target_include_directories(inicheck PRIVATE "${CMAKE_SOURCE_DIR}/code")
set_target_properties(inicheck PROPERTIES
    FOLDER "Tools"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tools-bin"
)
target_compile_options(inicheck PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Debug>:/MTd>>
    $<$<CXX_COMPILER_ID:MSVC>:$<$<CONFIG:Release>:/MT>>
    $<$<CXX_COMPILER_ID:MSVC>:/EHsc>
    $<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>
)

# The checker reports against the catalog the export script writes; a test runs it over a
# small sample so that the tool and the library keep agreeing.
add_test(NAME inicheck-cli
    COMMAND inicheck --catalog "${CMAKE_CURRENT_SOURCE_DIR}/sample-catalog.tsv" "${CMAKE_CURRENT_SOURCE_DIR}/sample.ini"
)
set_tests_properties(inicheck-cli PROPERTIES PASS_REGULAR_EXPRESSION "line 8: \\[MYTANK\\] Speed=fast: expected a whole number")
