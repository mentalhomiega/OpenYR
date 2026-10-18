# Keyboard defaults

This page gives each command's default key in Yuri's Revenge, the key our old builds bound, and what the current build does. The defaults are in `code/keybinddefaults.cpp`.

## Where the defaults come from

- gamemd's loader, `CommandClass::LoadHotkeys` (`00533d20`), has no fallback table. It reads `KEYBOARDMD.INI` through `CCFileClass`, binds each `[Hotkey]` name to the command class with that name, and returns 0 when the file is missing.
- The default is the copy of `KEYBOARDMD.INI` the game finds in its MIX archives. The engine's file lookup found it in a test run (`dumpfile KEYBOARDMD.INI` logged 1869 bytes). We did not identify the archive, because the game's MIX indexes are encrypted.
- A loose `KEYBOARDMD.INI` in the game folder is found before the copy in the archive, in our engine's lookup. We have not checked gamemd's lookup order. The Steam install of Red Alert II used here holds a loose copy, 1405 bytes, with a numpad layout (for example `DeployObject=97` and `TypeSelect=96`). If gamemd reads that copy, its players see the numpad keys.
- Our old builds had no defaults in code. Their saved files held the keys of the commands they bound. Each of those keys that the stock copy also binds has the stock key. The rest belong to OpenTS commands.

Keys are written as the Windows virtual-key code, in brackets. Shift adds 256, Control adds 512 and Alt adds 1024.

## Summary

Of the 95 Yuri's Revenge commands in the table, 72 keep the key our old builds bound, because that key matches the stock copy. Five commands we already had gain the key Yuri's Revenge gives them: `DefenseTab`, `InfantryTab`, `PlanningMode`, `StructureTab` and `UnitTab`. Four new commands work: `TypeSelect`, `CombatantSelect`, `VeterancyNav` and `MultiplayerDebug`. `HealthNav` is new and has no key. Ten new commands only report that their feature is missing from this build: `PlaceBeacon`, `Delete` and `Taunt_1` to `Taunt_8`. Three are not ported: `PageUser`, `CursorCheat` and `ExtraSyncChecking`.

No OpenTS command takes a key that a Yuri's Revenge command uses by default. The table gives every default a distinct key.

## Control schemes and triggers

A keyboard file's `[Defaults] Scheme=` picks the defaults: Classic (Yuri's Revenge's keys, the default for a file with no scheme) or Modern. The keyboard page switches the scheme and replaces every binding with the new scheme's defaults after the player confirms. Each command has a primary and a secondary key. Each key has a trigger: `Press`, `Double`, `Hold` or `Held`. The file format is in [KEYBOARDMD.INI](/formats/keyboard-ini/).

Modern differs from Classic in these bindings. Everything else is the same in both.

| Key | Modern | Classic |
| --- | --- | --- |
| A | `AttackMove`, a press | `ToggleAlliance` |
| V | `WaypointMode`, a press | none |
| Home | `CenterBase`, a press; H is its secondary key | `LeftSidebarUp` |
| End | none | `LeftSidebarDown` |
| F10 | `Options`, the secondary key of Esc | `Taunt_6` |
| Tab | `NextSidebarTab`, a press | map zoom (not a binding) |
| Up, Down, Left, Right | `ScrollNorth`, `ScrollSouth`, `ScrollWest`, `ScrollEast`, each `Held` | none |
| 1 to 9 and 0 | `TeamCenter_1` to `TeamCenter_10`, each a `Double` press of its group key | none |
| Left and Right arrow, on release | none (the sidebar pages stay on Page Up and Page Down) | `SidebarPageUp`, `SidebarPageDown` |

Modern keeps these Classic keys: Ctrl+1 to Ctrl+0 create a group, Shift adds to it, and 1 to 0 select it; Alt+1 to Alt+0 centre a group as a second key; S stops, G guards, D deploys, X scatters, Z plans; F1 to F4 jump to a bookmark and Ctrl+F1 to Ctrl+F4 store one; Space centres on the last event; Esc and F10 open the menu; Enter opens chat.

The four held modifiers are the same in both schemes: `ForceFire` on Control, `ForceMove` on Alt, `SelectModifier` on Shift and `QueueMove` on Q. A `Held` binding is active while its key is down. The game reads these four keys when it applies the bindings, so the mouse commands see the key a player bound.

A `Double` binding on a key with a `Press` binding makes that press wait for the double-press window, 250 ms by default. A key with only a `Press` fires at once. A `Hold` binding makes a quick tap of its key fire the press at the release, and a long press fire the hold at 400 ms. A `Held` binding fires when the key goes down and again every 40 ms while it stays down, which is how the arrow keys scroll the view.

Grid hotkeys for the sidebar are not in either scheme. They would take the Q, W, E and R tab keys and the A, S, D and X command keys.

## New command behaviour

- `TypeSelect` selects the player's units on screen whose types match the selection, when the key is released within half a second. A longer press does nothing, as in gamemd.
- `CombatantSelect` selects the player's armed units on screen. When they are all selected already, it selects every armed unit the player owns. A harvester counts as armed, since its stock weapon is a 20 mm gun. The decompile does not confirm gamemd's test for a combatant, so this rule may differ.
- `HealthNav` selects the red units on the first press, then yellow, then green, and then red again. A band with no unit is skipped. The units come from the current selection, or from every unit the player owns when nothing is selected. Later presses keep that source while the selection still holds the last result.
- `VeterancyNav` runs the same cycle over elite, veteran and green units.
- `PlaceBeacon` and `Delete` work on beacons, which this build does not have. In a single-player game they do nothing, as in gamemd. In a multiplayer game they report that beacons are not available.
- `Taunt_1` to `Taunt_8` play taunts in multiplayer games in gamemd. In a single-player game they do nothing. In a multiplayer game they report that taunts are not available.
- `MultiplayerDebug` toggles the multiplayer debug display. gamemd registers the command only when the game starts with `-MPDEBUG`, and nothing in its code turns the display on at start. This build always lists the command, and `-MPDEBUG` still starts the display shown. The display uses gamemd's labels. It does not show gamemd's `NFT`, `LNFT`, `Rout resp` or `MC resp` lines, and gamemd's display has no `FrameSendRate` line.
- `PageUser` sends an event whose gamemd identity the decompile does not settle. `CursorCheat` shows a cell readout, and `ExtraSyncChecking` toggles extra sync logging. We have not ported them.

## Upgrading existing keyboard files

- A file with `[Defaults] Version=2` is read as saved. A command the file does not name has no key.
- A file without that version is read as version 1, which is what the older builds wrote. Each start applies the upgrade in memory, and the file stays as it was until the keyboard dialog saves it. An entry that still holds its old key takes the current key, or loses its key when the current table gives none. A key the player set stays. A command the file does not name gets its current key when that key is free.
- With no keyboard file, each command gets its current key.
- The keyboard page's "Reset category" and "Reset all" buttons change the page's bindings to the scheme's defaults. Nothing is written until OK saves them, and Cancel discards them. The reset does not delete the loose file.

## Yuri's Revenge commands

| Command | Category | Yuri's Revenge default | Old default | gamemd class and `Execute` | Status |
| --- | --- | --- | --- | --- | --- |
| `AllToCheer` | Control | C (67) | C (67) | `AllToCheerCommandClass::Execute` `00536c00` | Kept |
| `CenterBase` | Selection | H (72) | H (72) | `CenterBaseCommandClass::Execute` `00536ea0` | Kept |
| `CenterOnRadarEvent` | Interface | Space (32) | Space (32) | `CenterREventCommandClass::Execute` `00537130` | Kept |
| `CenterView` | Selection | Numpad 5 (Num Lock off) (12) | Numpad 5 (Num Lock off) (12) | `CenterViewCommandClass::Execute` `00536e00` | Kept |
| `CombatantSelect` | Selection | P (80) | none | `CombatantSelectCommandClass::Execute` `005367f0` | New |
| `CursorCheat` | - | none | none | `CursorPositionCommandClass::Execute` `00537ef0` | Not ported |
| `DefenseTab` | Interface | W (87) | none | `SetDefenseTabCommandClass::Execute` `00535e50` | Added; the old build had no key |
| `Delete` | Interface | Numpad Decimal (110) | none | `DeleteCommandClass::Execute` `00537f90` | New; reports that the feature is not in this build |
| `DeployObject` | Control | D (68) | D (68) | `DeployCommandClass::Execute` `00536c80` | Kept |
| `ExtraSyncChecking` | - | none | none | `MultiplayerSyncCommandClass::Execute` `00537af0` | Not ported |
| `Follow` | Interface | F (70) | F (70) | `FollowCommandClass::Execute` `00537a10` | Kept |
| `GuardObject` | Control | G (71) | G (71) | `GuardCommandClass::Execute` `00536d00` | Kept |
| `HealthNav` | Selection | none | none | `HealthNavCommandClass::Execute` `00536950` | New; the stock copy gives no key |
| `InfantryTab` | Interface | E (69) | none | `SetInfantryTabCommandClass::Execute` `00535f10` | Added; the old build had no key |
| `LeftSidebarDown` | Interface | End (35) | End (35) | not located in the class files | Kept |
| `LeftSidebarUp` | Interface | Home (36) | Home (36) | not located in the class files | Kept |
| `MultiplayerDebug` | Debug | Ctrl+I (585) | none | `MultiplayerDebugCommandClass::Execute` `00537a90` | New; the display draws only in an IPX or Internet game |
| `NextObject` | Selection | N (78) | N (78) | `NextObjectCommandClass::Execute` `00536610` | Kept |
| `Options` | Interface | Esc (27) | Esc (27) | `OptionsCommandClass::Execute` `005372d0` | Kept |
| `PageUser` | - | U (85) | none | `PageUserCommandClass::Execute` `00537e50` | Not ported |
| `PlaceBeacon` | Interface | B (66) | none | `BeaconPlacementCommandClass::Execute` `005370a0` | New; reports that the feature is not in this build |
| `PlanningMode` | Interface | Z (90) | none | `PlanningModeCommandClass::Execute` `00536750` | Added; the old build had no key |
| `PreviousObject` | Selection | M (77) | M (77) | `PrevObjectCommandClass::Execute` `00536a80` | Kept |
| `RightSidebarDown` | Interface | Page Down (34) | Page Down (34) | not located in the class files | Kept |
| `RightSidebarUp` | Interface | Page Up (33) | Page Up (33) | not located in the class files | Kept |
| `ScatterObject` | Control | X (88) | X (88) | `ScatterCommandClass::Execute` `00536d80` | Kept |
| `ScreenCapture` | Interface | Shift+S (339) | Shift+S (339) | `ScreenCaptureCommandClass::Execute` `00537bc0` | Kept |
| `SetView1` | Interface | Ctrl+F1 (624) | Ctrl+F1 (624) | `SetView1CommandClass::Execute` `00537710` | Kept |
| `SetView2` | Interface | Ctrl+F2 (625) | Ctrl+F2 (625) | `SetView2CommandClass::Execute` `005377d0` | Kept |
| `SetView3` | Interface | Ctrl+F3 (626) | Ctrl+F3 (626) | `SetView3CommandClass::Execute` `00537890` | Kept |
| `SetView4` | Interface | Ctrl+F4 (627) | Ctrl+F4 (627) | `SetView4CommandClass::Execute` `00537950` | Kept |
| `SidebarDown` | Interface | Numpad 2 (98) | Numpad 2 (98) | `SidebarDownCommandClass::Execute` `00537240` | Kept |
| `SidebarPageDown` | Interface | Release of Right arrow (2087) | Release of Right arrow (2087) | not located in the class files | Kept |
| `SidebarPageUp` | Interface | Release of Left arrow (2085) | Release of Left arrow (2085) | not located in the class files | Kept |
| `SidebarUp` | Interface | Numpad 8 (104) | Numpad 8 (104) | `SidebarUpCommandClass::Execute` `005371b0` | Kept |
| `StopObject` | Control | S (83) | S (83) | `StopCommandClass::Execute` `00536b80` | Kept |
| `StructureTab` | Interface | Q (81) | none | `SetStructureTabCommandClass::Execute` `00535d90` | Added; the old build had no key |
| `Taunt_1` | Chat | F5 (116) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_2` | Chat | F6 (117) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_3` | Chat | F7 (118) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_4` | Chat | F8 (119) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_5` | Chat | F9 (120) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_6` | Chat | F10 (121) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_7` | Chat | F11 (122) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `Taunt_8` | Chat | F12 (123) | none | `TauntCommandClass::Execute` `00536350` | New; reports that the feature is not in this build |
| `TeamAddSelect_1` | Team | Shift+1 (305) | Shift+1 (305) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_2` | Team | Shift+2 (306) | Shift+2 (306) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_3` | Team | Shift+3 (307) | Shift+3 (307) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_4` | Team | Shift+4 (308) | Shift+4 (308) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_5` | Team | Shift+5 (309) | Shift+5 (309) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_6` | Team | Shift+6 (310) | Shift+6 (310) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_7` | Team | Shift+7 (311) | Shift+7 (311) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_8` | Team | Shift+8 (312) | Shift+8 (312) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_9` | Team | Shift+9 (313) | Shift+9 (313) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamAddSelect_10` | Team | Shift+0 (304) | Shift+0 (304) | `AddTeamCommandClass::Execute` `00536170` | Kept |
| `TeamCenter_1` | Team | Alt+1 (1073) | Alt+1 (1073) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_2` | Team | Alt+2 (1074) | Alt+2 (1074) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_3` | Team | Alt+3 (1075) | Alt+3 (1075) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_4` | Team | Alt+4 (1076) | Alt+4 (1076) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_5` | Team | Alt+5 (1077) | Alt+5 (1077) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_6` | Team | Alt+6 (1078) | Alt+6 (1078) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_7` | Team | Alt+7 (1079) | Alt+7 (1079) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_8` | Team | Alt+8 (1080) | Alt+8 (1080) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_9` | Team | Alt+9 (1081) | Alt+9 (1081) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCenter_10` | Team | Alt+0 (1072) | Alt+0 (1072) | `CenterTeamCommandClass::Execute` `00536260` | Kept |
| `TeamCreate_1` | Team | Ctrl+1 (561) | Ctrl+1 (561) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_2` | Team | Ctrl+2 (562) | Ctrl+2 (562) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_3` | Team | Ctrl+3 (563) | Ctrl+3 (563) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_4` | Team | Ctrl+4 (564) | Ctrl+4 (564) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_5` | Team | Ctrl+5 (565) | Ctrl+5 (565) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_6` | Team | Ctrl+6 (566) | Ctrl+6 (566) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_7` | Team | Ctrl+7 (567) | Ctrl+7 (567) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_8` | Team | Ctrl+8 (568) | Ctrl+8 (568) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_9` | Team | Ctrl+9 (569) | Ctrl+9 (569) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamCreate_10` | Team | Ctrl+0 (560) | Ctrl+0 (560) | `CreateTeamCommandClass::Execute` `00535cc0` | Kept |
| `TeamSelect_1` | Team | 1 (49) | 1 (49) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_2` | Team | 2 (50) | 2 (50) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_3` | Team | 3 (51) | 3 (51) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_4` | Team | 4 (52) | 4 (52) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_5` | Team | 5 (53) | 5 (53) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_6` | Team | 6 (54) | 6 (54) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_7` | Team | 7 (55) | 7 (55) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_8` | Team | 8 (56) | 8 (56) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_9` | Team | 9 (57) | 9 (57) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `TeamSelect_10` | Team | 0 (48) | 0 (48) | `SelectTeamCommandClass::Execute` `00536080` | Kept |
| `ToggleAlliance` | Control | A (65) | A (65) | `AllianceCommandClass::Execute` `00536f20` | Kept |
| `ToggleRepair` | Interface | K (75) | K (75) | `ToggleRepairCommandClass::Execute` `00536fa0` | Kept |
| `ToggleSell` | Interface | L (76) | L (76) | `ToggleSellCommandClass::Execute` `00537020` | Kept |
| `TypeSelect` | Selection | T (84) | none | `TypeSelectCommandClass::Execute` `005368b0` | New |
| `UnitTab` | Interface | R (82) | none | `SetUnitTabCommandClass::Execute` `00535fa0` | Added; the old build had no key |
| `VeterancyNav` | Selection | Y (89) | none | `VeterancyNavCommandClass::Execute` `005369f0` | New |
| `View1` | Interface | F1 (112) | F1 (112) | `View1CommandClass::Execute` `00537350` | Kept |
| `View2` | Interface | F2 (113) | F2 (113) | `View2CommandClass::Execute` `00537440` | Kept |
| `View3` | Interface | F3 (114) | F3 (114) | `View3CommandClass::Execute` `00537530` | Kept |
| `View4` | Interface | F4 (115) | F4 (115) | `View4CommandClass::Execute` `00537620` | Kept |

## OpenTS commands

| Command | Category | Yuri's Revenge default | Old default | Status |
| --- | --- | --- | --- | --- |
| `ChatToAll` | Chat | none | Enter (13) | OpenTS only; default kept |
| `ChatToAllies` | Chat | none | Backspace (8) | OpenTS only; default kept |
| `DebugChargeSuperweapons` | Debug | none | none | OpenTS only; no default |
| `DebugDestroySelected` | Debug | none | none | OpenTS only; no default |
| `DebugFastForward` | Debug | none | none | OpenTS only; no default |
| `DebugFullPower` | Debug | none | none | OpenTS only; no default |
| `DebugHealSelected` | Debug | none | none | OpenTS only; no default |
| `DebugInfiniteMoney` | Debug | none | none | OpenTS only; no default |
| `DebugInstantBuild` | Debug | none | none | OpenTS only; no default |
| `DebugInvulnerable` | Debug | none | none | OpenTS only; no default |
| `DebugLoseMission` | Debug | none | none | OpenTS only; no default |
| `DebugPromoteSelected` | Debug | none | none | OpenTS only; no default |
| `DebugRevealMap` | Debug | none | none | OpenTS only; no default |
| `DebugUnlockAll` | Debug | none | none | OpenTS only; no default |
| `DebugWinMission` | Debug | none | none | OpenTS only; no default |
| `DeleteWaypoint` | Interface | none | Delete (46) | OpenTS only; keeps Delete (46), which Init_Commands forces to it |
| `HealthFilter` | Selection | none | none | OpenTS only; no default |
| `HealthFilterAddLower` | Selection | none | none | OpenTS only; no default |
| `JumpCameraEast` | Interface | none | none | OpenTS only; no default |
| `JumpCameraNorth` | Interface | none | none | OpenTS only; no default |
| `JumpCameraSouth` | Interface | none | none | OpenTS only; no default |
| `JumpCameraWest` | Interface | none | none | OpenTS only; no default |
| `LeftSidebarPageDown` | Interface | none | none | OpenTS only; no default |
| `LeftSidebarPageUp` | Interface | none | none | OpenTS only; no default |
| `ManualPlace` | Interface | none | none | OpenTS only; no default |
| `NextTheme` | Interface | none | none | OpenTS only; no default |
| `PrevTheme` | Interface | none | none | OpenTS only; no default |
| `QuickLoad` | Interface | none | none | OpenTS only; no default |
| `QuickSave` | Interface | none | none | OpenTS only; no default |
| `ReloadRules` | Interface | none | Ctrl+Shift+R (850) | OpenTS only; default kept |
| `RepeatLastAircraft` | Interface | none | none | OpenTS only; no default |
| `RepeatLastBuilding` | Interface | none | none | OpenTS only; no default |
| `RepeatLastInfantry` | Interface | none | none | OpenTS only; no default |
| `RepeatLastUnit` | Interface | none | none | OpenTS only; no default |
| `RightSidebarPageDown` | Interface | none | none | OpenTS only; no default |
| `RightSidebarPageUp` | Interface | none | none | OpenTS only; no default |
| `ScrollEast` | Interface | none | none | OpenTS only; no default |
| `ScrollNorth` | Interface | none | none | OpenTS only; no default |
| `ScrollNorthEast` | Interface | none | none | OpenTS only; no default |
| `ScrollNorthWest` | Interface | none | none | OpenTS only; no default |
| `ScrollSouth` | Interface | none | none | OpenTS only; no default |
| `ScrollSouthEast` | Interface | none | none | OpenTS only; no default |
| `ScrollSouthWest` | Interface | none | none | OpenTS only; no default |
| `ScrollWest` | Interface | none | none | OpenTS only; no default |
| `SelectOneLess` | Selection | none | none | OpenTS only; no default |
| `SelectType` | Selection | none | none | OpenTS only; no default |
| `SelectView` | Selection | none | none | OpenTS only; no default |
| `TeamAddTo_1` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_2` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_3` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_4` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_5` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_6` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_7` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_8` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_9` | Team | none | none | OpenTS only; no default |
| `TeamAddTo_10` | Team | none | none | OpenTS only; no default |
| `TogglePower` | Interface | none | none | OpenTS only; no default |
| `ToggleRadar` | Interface | none | none | OpenTS only; no default |
| `VeterancyFilter` | Selection | none | none | OpenTS only; no default |
| `VeterancyFilterAddLower` | Selection | none | none | OpenTS only; no default |
| `WaypointMode` | Interface | none | none | OpenTS only; no default |
## Stock keys for names the command classes do not register

The stock copy also binds these names. gamemd's command classes do not register them, so this build leaves them unbound. Their names, such as `FileNew` and `CopyBlock`, look like map-editor commands, which share the file. We did not check that.

`RaiseCell` 106, `LowerCell` 109, `DeleteObject` 187, `SuperExplosion` 192, `ToggleMono` 333, `NukeExplosion` 334, `PrevMonoPage` 378, `NextMonoPage` 379, `CopyBlock` 579, `ForceLose` 588, `FreeMoney` 589, `FileNew` 590, `FileOpen` 591, `BuildCheat` 593, `FileSave` 595, `PasteBlock` 598, `ForceWin` 599, `ToggleMarbleMadness` 1101

