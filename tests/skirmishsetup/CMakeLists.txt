# Links the skirmish lobby's presenter with the same engine sources and stubs as the uishell harness,
# so a remembered setup is restored through the code the lobby runs. Needs no game data.
opents_add_test(SkirmishSetup
    NAME skirmishsetup
    INCLUDES ${OPENTS_GENERATED_DIR}
    SOURCES skirmishsetup.cpp
        ../uishell/uicolorstub.cpp
        ../uishell/uidevstub.cpp
        ../uishell/uiscriptstub.cpp
        ../uishell/uitexturestub.cpp
    ENGINE
        dbgprint.cpp
        ui/uiscreen.cpp
        ui/uiinput.cpp
        ui/uishell.cpp
        ui/uireveal.cpp
        ui/uisettingpreview.cpp
        utf8.cpp
        hsv.cpp
        modchoice.cpp
        playercolor.cpp
        ui/screens/colorpick/uicolorpick.cpp
        rgb.cpp
        ui/rml/rmlfont.cpp
        ui/rml/rmlfontfon.cpp
        ui/rml/rmlfontsheet.cpp
        ui/rml/rmlimage.cpp
        ui/rml/rmlkeys.cpp
        ui/rml/rmlrenderbase.cpp
        ui/rml/rmlrendermath.cpp
        ui/rml/rmlsettingpreview.cpp
        ui/rml/rmlsurface.cpp
        ui/rml/rmlsystem.cpp
        ui/rml/rmlview.cpp
        ui/screens/abort/uiabort.cpp
        ui/screens/campaign/uicampaign.cpp
        ui/screens/classicopt/uiclassicopt.cpp
        ui/screens/display/uidisplay.cpp
        ui/screens/gamectrl/uigamectrl.cpp
        ui/screens/gameopt/uigameopt.cpp
        ui/screens/keyboard/uikeyboard.cpp
        keybinddefaults.cpp
        keybindings.cpp
        ui/screens/mainopt/uimainopt.cpp
        ui/screens/desync/uidesync.cpp
        ui/screens/mapgen/uimapgen.cpp
        ui/screens/missionsel/uimissionsel.cpp
        ui/screens/menu/uimenu.cpp
        ui/screens/mods/uimods.cpp
        ui/screens/movies/uimovies.cpp
        ui/screens/reconnect/uireconnect.cpp
        ui/screens/restate/uirestate.cpp
        ui/screens/msgbox/uimsgbox.cpp
        ui/screens/netlobby/uinetlobby.cpp
        ui/screens/savegame/uisavegame.cpp
        ui/screens/scenario/uiscenario.cpp
        ui/screens/skirmish/uiskirmish.cpp
        ui/screens/sound/uisound.cpp
        ui/screens/version/uiversion.cpp
        ui/screens/waitbox/uiwaitbox.cpp
    INCLUDES "${OPENTS_GENERATED_DIR}"
    DEFINITIONS WIN32 _WINDOWS NOMINMAX "OPENTS_UI_DIR=\"${CMAKE_SOURCE_DIR}/ui\""
    LIBRARIES RmlUi::Core freetype imgui
    STAMP
)
