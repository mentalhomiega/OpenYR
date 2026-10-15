# The harness compiles the browser's presenter and its row layout, which need no RmlUi, and the presenter base
# class. The battle-file scenario reader is header-only.

opents_add_test(MissionBrowser
    NAME missionbrowser
    SOURCES missionbrowsertest.cpp
    ENGINE
        ui/uiscreen.cpp
        ui/screens/missionbrowser/uimissionbrowser.cpp
)
