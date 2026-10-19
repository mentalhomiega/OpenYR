---
format_id: ui-ini
title: UI.INI
summary: Sets how a selected object's order lines, a firing vehicle's sighting laser and a control group number are drawn, which title menu buttons are shown, and which player colors the lobbies offer.
kind: file
source_files:
  - code/uicontrol.cpp
  - code/init.cpp
filenames:
  - UI.INI
key_scopes:
  - file: ui.ini
    section:
      kind: literal
      name: Ingame
  - file: ui.ini
    section:
      kind: literal
      name: Pips
related:
  - type: system
    id: action-lines
  - type: format
    id: opents-ini
  - type: using
    id: configuration-files
---

The file is optional and every key has a default, so a file names only what it changes. The order-line and sighting-laser keys go in `[Ingame]`, and the control group number keys go in `[Pips]`. The values below are examples.

```ini title="UI.INI"
[Ingame]
AlwaysShowActionLines=yes
MovementLineDashed=yes
MovementLineColor=0,255,0
NavComQueueLineThick=yes

[Pips]
UnitWithPipGroupNumberOffset=-4,-15
```

Write a color as three numbers from 0 to 255 for red, green and blue, separated by commas. A color value that does not start with three comma-separated numbers keeps the default. Anything after the third number is ignored. [Action lines](/systems/action-lines/) explains what each line is and when it is drawn.

## Title menu buttons

`[UISettings]` changes what the title menu buttons do. Each key names one button and takes an action. A key that is absent, or any other value, leaves the button as it is.

| Key | Menu | Button |
| --- | --- | --- |
| `SinglePlayerButton` | Main | Single Player |
| `NetworkButton` | Main | Network |
| `MoviesAndCreditsButton` | Main | Movies & Credits |
| `CampaignButton` | Single Player | New Campaign |
| `SkirmishButton` | Single Player | Skirmish |
| `SneakPeeksButton` | Movies & Credits | Sneak Peeks |
| `PlayMoviesButton` | Movies & Credits | Play Movies |
| `ViewCreditsButton` | Movies & Credits | View Credits |

| Action | Effect |
| --- | --- |
| `default` | The button works as usual. |
| `disable` | The button stays in the menu and cannot be clicked. |
| `hide` | The button is left out of the menu. |
| `message` | A click shows a message box with the string table text `TXT_<KEY>_MSG`, where the key is the button's key in capitals, such as `TXT_VIEWCREDITSBUTTON_MSG`, and the menu shows again. |
| `credits` | A click starts the credits. |
| `sneakpeek` | A click starts the sneak peek movie. |

```ini title="UIMD.INI"
[UISettings]
CampaignButton=hide
ViewCreditsButton=message
```

The menus read the actions each time the player enters them, from the file read last. A mod's [`FollowMenuButtons=no`](/formats/mod-ini/) makes the menus ignore them. The CnCNet button has no key, and a key for the Westwood Online button, `WWOnlineButton`, is read but there is no such button to change.

## Lobby colors

`[Colors]` lists the colors the skirmish and network lobbies offer. Without the section the lobbies offer the eight stock colors. `Count` is the number of colors listed, from 8 to 16, and a value outside that range is held to it. Slot `N`, from 1 to `Count`, takes three keys:

| Key | Effect |
| --- | --- |
| `SlotN.ColorScheme` | The scheme from the rules' `[Colors]` that a player who picks the slot is drawn in. |
| `SlotN.DisplayColor` | The swatch the lobby shows, as red, green and blue from 0 to 255. |
| `SlotN.Tooltip` | A string table label for the name the lobby shows. |

`Observer.ColorScheme` sets the scheme of a spectator. A key left out keeps the default for its slot. The first eight slots default to the stock colors, their swatches and their names. Slots 9 to 14 default to the schemes `NeonBlue`, `LightBlue`, `Yellow`, `Green`, `Red` and `Grey`, and slots 15 and 16 to `LightGrey`. A slot with a scheme and no tooltip is named after the scheme, with its words apart, so `LightBlue` shows as Light Blue. A scheme the rules do not define leaves the slot in the stock scheme for its position.

```ini title="UIMD.INI"
[Colors]
Count=14
Slot1.ColorScheme=Teal
Slot1.DisplayColor=0,128,128
Slot2.ColorScheme=Red
Slot2.DisplayColor=255,0,0
```

The lobbies, the color picker and the game use the same list, so a color index means the same slot everywhere. The color-blind safe view replaces only the first eight slots; a later slot keeps its own scheme.

## Control group numbers

An object whose pips are drawn also shows its group number when its group is one of the ten the player's control groups use. The number is the group's key, `1` to `9`, or `0` for the tenth group. Eight `[Pips]` keys set where it is printed, one pair for each kind of object:

| Object | Without pips | With pips |
| --- | --- | --- |
| Vehicle | [`UnitGroupNumberOffset`](/keys/unitgroupnumberoffset/) | [`UnitWithPipGroupNumberOffset`](/keys/unitwithpipgroupnumberoffset/) |
| Infantry | [`InfantryGroupNumberOffset`](/keys/infantrygroupnumberoffset/) | [`InfantryWithPipGroupNumberOffset`](/keys/infantrywithpipgroupnumberoffset/) |
| Structure | [`BuildingGroupNumberOffset`](/keys/buildinggroupnumberoffset/) | [`BuildingWithPipGroupNumberOffset`](/keys/buildingwithpipgroupnumberoffset/) |
| Aircraft | [`AircraftGroupNumberOffset`](/keys/aircraftgroupnumberoffset/) | [`AircraftWithPipGroupNumberOffset`](/keys/aircraftwithpipgroupnumberoffset/) |

An object uses the "with pips" key when its type's pip row, which [`MaxPips`](/keys/maxpips/) describes, has a length other than zero. A type with [`PipScale=Ammo`](/keys/pipscale/) and the default unlimited [`Ammo`](/keys/ammo/) counts as having pips even though it draws none.

Each value is two whole numbers, a horizontal and a vertical offset in pixels, separated by a comma. Positive values move the number right and down. The offset is measured from the point the object's pips are laid out from, and the number's top-left corner is placed there:

- for a structure, or a vehicle with [`IsCoreDefender=yes`](/keys/iscoredefender/#scope-unittype), that point is the left-hand corner of its footprint at ground level;
- for any other object, it is 10 pixels left of and 10 pixels below the object's position.

The defaults are `-4,-4` without pips and `-4,-8` with pips. A value that does not start with two comma-separated numbers keeps the default, and anything after the second number is ignored.

Vinifera reads the same eight keys from `[Ingame]`, so move them to `[Pips]` when reusing a Vinifera file. Setting all eight to `-8,-33` gives the position Twisted Insurrection uses.

## When the file is read

The game reads the file at startup, after it registers the game archives. It reads the file again each time a scenario or saved game loads, after mounting the archives of the player's side, so a copy inside a side's archive applies to games played as that side. It reads the file once more when a match ends and the menus return, after mounting the [no-side archives](/formats/mix/#theater-side-and-speech-archives). Each read starts from the defaults, so only the keys in the copy read last take effect.

The file is opened through the game's file search, so it may be a loose file in any folder the game searches or be inside an archive. A loose copy is used ahead of an archived one. [OPENTS.INI](/formats/opents-ini/#the-order-files-are-searched-for-in) lists the folders and the order they are searched in, and its [`[Files]` section](/formats/opents-ini/#the-files-it-reads) can change the file name.
