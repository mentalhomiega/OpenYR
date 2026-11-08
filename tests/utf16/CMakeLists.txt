# The helper is header only, so the harness compiles no engine source. It lives outside code/ so that the recursive glob building OpenTS cannot pick this target's entry point up.

opents_add_test(UTF16Contract
    NAME utf16contract
    SOURCES utf16contract.cpp
)
