# The slot rules are header-only, so the harness compiles nothing from code/. It supplies its
# own stand-in contact; the templates compare pointers and read nothing else.

opents_add_test(RadioSlots
    NAME radioslots
    SOURCES radioslotstest.cpp
)
