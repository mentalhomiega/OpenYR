# Multiplayer game modes

This note records how gamemd.exe (Yuri's Revenge 1.001) selects and applies its multiplayer modes, and what the port does with them. Addresses are gamemd virtual addresses in the image based at `0x400000`. They come from the binary and its decompile, not from the port.

## Modes

The lobby lists the modes that `MPModesMD.ini` defines. Each line gives one mode its index, which the match stores as `SessionClass::Config.MPModeIndex`.

| Index | Lobby name | Class (vtable) | Overrides file | Map filter | Random maps | AlliesAllowed | MustAlly |
| --- | --- | --- | --- | --- | --- | --- | --- |
| 1 | Battle | Battle (`0x7EE184`) | `MPBattleMD.ini` | `standard` | yes | yes | no |
| 2 | FreeForAll | FreeForAll (`0x7EE424`) | `MPFreeForAllMD.ini` | `standard` | yes | no | no |
| 3 | Cooperative | Cooperative (`0x7EE27C`) | `MPCoopMD.ini` | `cooperative` | no | no | no |
| 4 | UnholyAlliance | Unholy (`0x7EE814`) | `MPUnholyMD.ini` | `standard` | no | yes | no |
| 5 | Megawealth | ManBattle (`0x7EE50C`) | `MPMWMD.ini` | `megawealth` | no | yes | no |
| 6 | Duel | ManBattle | `MPDuelMD.ini` | `duel` | no | yes | no |
| 7 | MeatGrinder | ManBattle | `MPMeatMD.ini` | `meatgrind` | no | yes | no |
| 8 | NavalWar | ManBattle | `MPNavalMD.ini` | `navalwar` | no | yes | no |
| 9 | TeamGame | Battle | `MPTeamMD.ini` | `teamgame` | no | yes | yes |

The class column is the section the line sits in. The Megawealth class (`0x7EE5F4`) is not constructed by any initializer, so index 5 is a ManBattle object. The alliance columns come from each overrides file (see below).

## MPModesMD.ini

The loader is `0x5D7590`. Its file name string is at `0x830A18`.

`GameModes_Init` (`0x5D7CE0`) calls the loader six times, in this order: Battle, ManBattle, Siege, Unholy, FreeForAll, Cooperative. Each call names one section and the initializer for its class. The creators are `0x5D8170` (Battle), `0x5D81B0` (ManBattle), `0x5D81F0` (Siege), `0x5D8230` (Unholy), `0x5D8270` (FreeForAll) and `0x5D82B0` (Cooperative).

The sections and keys the loader reads:

- **Section.** The section name is the class. A section that the file lacks has no modes. The shipped file has no `[Siege]` section, so the lobby cannot select Siege.
- **Key.** The key is the mode's index, read as a decimal number with `atoi`. The index names the mode everywhere else, including `SessionClass::Config.MPModeIndex` and the `GameMode` key of a launch file.
- **Value.** The value has five comma-separated fields, which the loader passes to the class constructor in this order:
  1. the lobby's display key (`GUI:`), stored at `this+0x20`;
  2. the tooltip key (`STT:`), stored at `this+0x24`;
  3. the overrides file name, stored at `this+0x2C` and opened by the constructor;
  4. the map filter, stored at `this+0x30`;
  5. the random-map flag, stored at `this+0x34` as `AIAllowed`. The loader turns it off only for the text `false`, in any case.
- **Skipped lines.** The port skips a line unless its first four fields are non-empty (`code/mpmodeparse.cpp`). The loader's own checks cover some of those fields; the exact set was not confirmed field by field.

The loader keeps the modes sorted by index. `MPGameModeClass::Get` (`0x5D5F30`) finds a mode by index, and the default mode is the first in that list (`0x5D5E10`), which is Battle in the shipped file.

## Overrides files

The constructor (`0x5D5B60`) opens the overrides file named by field 3. It reads these keys from `[MultiplayerDialogSettings]`:

| Key | Default | Use |
| --- | --- | --- |
| `AlliesAllowed` | yes | Stored at `+0x3C`. |
| `MustAlly` | no | Stored at `+0x3F`. Cleared when `AlliesAllowed` is no. |
| `WonlineTournamentAllowed` | yes | Read; the port does not use it. |
| `WonlineClanTournamentAllowed` | yes | Read; the port does not use it. |

A file without the section keeps the defaults. The Team Game file also sets `AllyChangeAllowed=no`, which the constructor does not read.

`RulesClass::Init` (`0x6686C0`) also reads the whole overrides file as rules, through `RulesClass::Read_File`, after the rules file and before the map. It does this in every game except a campaign mission. The port does the same at the end of `RulesClass::Initialize`.

The overrides files in the shipped data set `AlliesAllowed=no` for FreeForAll and Cooperative, and `AlliesAllowed=yes` for the other modes. Only Team Game sets `MustAlly=yes`. Each file names its mode in a header comment, which is how the files map to modes above.

## Virtual slots

Each mode's vtable has 52 entries that point at code (`0`–`51`). The table gives each slot's base implementation and every mode whose vtable differs from it. The offset column is the byte offset, which is the name the decompile uses for some slots (`vt_entry_NN`). The decompile's `vftable[N]` is slot `N`.

Base addresses are from `MPGameModeClass`'s vtable at `0x7EED60`. Overrides list the mode and address.

| Slot | Offset | Base | Base behaviour | Overridden by |
| --- | --- | --- | --- | --- |
| 0 | `00` | `5D7F20` | Destructor: frees the team list and the INI. | Every class's destructor calls it. |
| 1 | `04` | `5C0E30` | Returns false. | Cooperative `5C4EE0` returns true. |
| 2 | `08` | `5C0E40` | Returns false. | Cooperative `5C4EF0` returns true. |
| 3 | `0C` | `5C0E50` | Returns true. | None. |
| 4 | `10` | `5D62C0` | Sets each start spot's index to its position, or to -1 when `0x696F10` returns -3. | FreeForAll `5C5D30` jumps to base. Siege `5CA680` sets the first spot to 1 and the others to 2. Unholy `5CB3F0` turns `Bases` on and returns true. Cooperative `5C18A0`. |
| 5 | `14` | `5D6310` | Returns true for its argument. | FreeForAll `5C5D40` checks the base, then renumbers. Siege `5CA6D0` requires one defender and at least one attacker. Unholy `5CB400` requires `Bases` and the base check. Cooperative `5C1D80`. |
| 6 | `18` | `5D6340` | Returns true. | FreeForAll `5C5D90` checks the base, then renumbers. Siege `5CA7D0` jumps to base. Unholy `5CB430` turns `Bases` on and returns true. Cooperative `5C1DC0`. |
| 7 | `1C` | `5D67A0` | Returns true. | Cooperative `5C17B0`. |
| 8 | `20` | `5C0E60` | No-op. The lobby calls it on the mode it selects. | Cooperative `5C1A10`. |
| 9 | `24` | `5C0E70` | No-op. The lobby calls it after a map is selected. | Cooperative `5C1CB0` sets a byte at `+0x340`. |
| 10 | `28` | `5D5DB0` | Returns -2. | Cooperative `5C1FA0`. |
| 11 | `2C` | `5D5DC0` | Returns 0 when `MustAlly`, else -2. The lobby stores it as a computer seat's team in some paths. | None. |
| 12 | `30` | `5D5DD0` | Returns 3 when `AlliesAllowed`, else -2. The lobby stores it as a new computer seat's team. | None. |
| 13 | `34` | `5D5DE0` | `CanAllyWith(idx)`: false for -2 under `MustAlly`; true for -1 to 3; false otherwise. | None. |
| 14 | `38` | `5C0E80` | No-op. | Cooperative `5C2640`. |
| 15 | `3C` | `5D6350` | Returns `AIAllowed`. Slot 17 calls it for a random map. | Siege `5CA7E0` returns false. Cooperative `5C3700` returns false. |
| 16 | `40` | `5D6360` | Returns false. The lobby calls it on selection and ignores the result. | Battle `5C0E20` and FreeForAll `5C5E30` return true. |
| 17 | `44` | `5D6370` | `FirstValidMapIndex`: the first multiplayer map the mode lists. A random map passes through slot 15. A fixed map passes the map filter (`0x69AE10`), unless the map's byte at `+0x17C` is clear and `0x77D940` returns true. | Cooperative `5C1CC0`. |
| 18 | `48` | `5D6450` | Fills the team dropdown. | None. |
| 19 | `4C` | `5D64C0` | Fills the team dropdown control. | Cooperative `5C3B10`. |
| 20 | `50` | `5D6540` | Fills the team dropdown for one seat. | FreeForAll `5C5DD0` adds its team entries. Cooperative `5C2CE0`. |
| 21 | `54` | `5C0EB0` | Returns false. Frees its first argument when the second is set. | None. |
| 22 | `58` | `5C0E90` | Same as slot 21. | Cooperative `5C2040`. |
| 23 | `5C` | `5C0ED0` | Returns true. | Cooperative `5C2100`. |
| 24 | `60` | `5D6320` | Returns true. The lobby calls it last on selection. | Cooperative `5C2680` returns false when `+0x40` is null. |
| 25 | `64` | `5D6330` | Returns true. | Cooperative `5C2710`. |
| 26 | `68` | `5C0EE0` | Returns true. | Cooperative `5C30C0`. |
| 27 | `6C` | `5D6430` | `RandomHumanCountryIndex`: returns a random index from `0x69B670`. | Cooperative `5C34F0`. |
| 28 | `70` | `5D6440` | Calls slot 27. | Cooperative `5C35F0`. |
| 29 | `74` | `5C0EF0` | No-op. | Cooperative `5C2A60`. |
| 30 | `78` | `5C0F00` | No-op. | Cooperative `5C21A0`. |
| 31 | `7C` | `5D6790` | `UnfixAlliances`: clears the fixed-alliance flag (`SpecialFlags` bit `0x400`) and returns true. The lobby calls it on selection. | Siege `5CA7F0` jumps to base. Cooperative `5C2CB0`. |
| 32 | `80` | `5D6BE0` | `StartingPositionsToHouseBaseCells`: moves each house with a start to its base cell. | None. |
| 33 | `84` | `5D6C70` | `StartingPositionsToHouseBaseCells2`: gives unplaced houses a start, through slot 49. | Siege `5CA800` takes the defender's spot first, then the attackers'. Cooperative `5C2EF0`. |
| 34 | `88` | `5D74A0` | `AllyTeams`: allies each pair of playing houses that share a team value other than -1 or -2. | Cooperative `5C3220`. |
| 35 | `8C` | `5D7570` | Returns true. | Cooperative `5C36F0` jumps to base. Megawealth `5C9430` calls `0x577F30(0)` and returns true. |
| 36 | `90` | `5C0F10` | No-op. | Cooperative `5C3F80`. |
| 37 | `94` | `5C0F20` | No-op. | Cooperative `5C4110`. |
| 38 | `98` | `5C0F30` | Returns -1. The lobby calls it on selection and ignores the result. | Cooperative `5C42C0` returns 2. |
| 39 | `9C` | `5C0F40` | No-op. The lobby calls it on the mode being replaced. | Cooperative `5C3F30`. |
| 40 | `A0` | `5C0F50` | No-op. | Cooperative `5C3710`. |
| 41 | `A4` | `5C0F60` | No-op. | Cooperative `5C44E0`. |
| 42 | `A8` | `5C0F70` | Returns false. | Cooperative `5C42D0`. |
| 43 | `AC` | `5C0F80` | No-op. | Cooperative `5C46E0`. |
| 44 | `B0` | `5C0F90` | No-op. | Cooperative `5C4CA0`. |
| 45 | `B4` | `5C0FA0` | Returns false. Frees its first argument when the second is set. | Cooperative `5C4E90`. |
| 46 | `B8` | `5C0FC0` | Returns 0. | Cooperative `5C4670`: when the campaign's last mission is won, it returns the persistent accomplishment bit for that mission. |
| 47 | `BC` | `5C0FD0` | Returns true. The lobby passes the result to a validity check. | Cooperative `5C4F00` returns false. |
| 48 | `C0` | `5D6690` | `CreateMPTeams`: builds the observer team and the player teams. | Siege `5CA9B0` builds observer, defender and attacker teams. |
| 49 | `C4` | `5D6890` | `AssignStartingPositionsToHouse`: picks one house's start cell. | Cooperative `5C2DE0`. |
| 50 | `C8` | `5D7030` | `SpawnBaseUnits`: gives the house its own base unit when `Bases` is on. | Siege `5CAAC0` checks, then calls base. Unholy `5CB440` places every base unit in the rules. |
| 51 | `CC` | `5D70F0` | `GenerateStartingUnits`: spends the starting credits on random units. | None. |

Slots 16, 38 and 47 have no identified purpose. The lobby calls them, and the table gives what each returns.

## Cooperative

Slots 1 and 2 return true only for Cooperative, so a call to either is a Cooperative test. `MPGameMode` is checked for null first, and the win, loss and briefing paths below also require `SessionClass::GameMode` 3 or 4 (a game on a network or online). The port's `MPMode_Is_Cooperative` is the mode test alone, and `MPMode_Is_Cooperative_Match` adds the network test (`GAME_IPX` or `GAME_INTERNET`).

| Where | Address | What gamemd does in Cooperative | Port |
| --- | --- | --- | --- |
| `HouseClass::AcceptDefeat` | `0x4FC0B0` | After the game-over test, a defeated local player wins when a living house is a person. Every other mode loses. | `MPlayer_Defeated`. The test is the port's `all_allies` test; a living allied person is enough, not exactly one. |
| `HouseClass::Update` | `0x4F86A5` | A win flagged on any person's house makes the local player win. | Ported; no stock map or rule flags another person's house. |
| `HouseClass::AllyAIHouses` | `0x501640` | The computer houses band together when `AlliesAllowed` is on or the mode is Cooperative. | `Computer_Paranoid`. |
| `RadarClass::Update` | `0x653850` | The diplomacy button opens the mission briefing dialog (special dialog 9) in Cooperative and in a campaign. Every other multiplayer game opens special dialog 8. | Ported. |
| `LoadProgressManager::Draw` | `0x553311`, `0x553E54` | The campaign's picture from `COOPCAMPMD.INI`; the description starts at y 124 on the 800 layout. | Ported. |
| Slot 33, `0x5C2EF0`, with slot 49, `0x5C2DE0` | | A person draws among the first `NumCoopHumanStartSpots` waypoints; a computer player takes the first free one after them. | `Assign_Start_Positions`. |
| Slot 34, `0x5C3220` | | Every person is allied with every other person, and every computer player with every other. | `Assign_Houses`. |
| Slot 31, `0x5C2CB0` | | Does not clear the map's `FixedAlliance` flag, as the base does. | Not needed; the stock maps set `FixedAlliance=no`. |
| Slot 40, `0x5C3710`, from `0x55CFD0` | | At the end of a game, when a person is not defeated, advances the campaign to its next map, saves the progress and shows a message. It then points the scenario at the campaign's current map and sets `SkipScore` and `SkipMapSelect`. | Not ported. |
| `Do_Win` `0x685670`, the defeat path `0x685DC0`, slots 41 to 43 | | Replace the multiplayer score and lobby return with a campaign dialog; a win starts the campaign's next mission and a loss starts the same mission again, through the lobby's waiting room. | Not ported. |
| `MapClass::Reveal` and the other reveal paths | `0x577D90`, `0x5675E5`, `0x5679DF`, `0x577F81` | A game on a network that is not Cooperative leaves three marker cells unrevealed; Cooperative reveals every cell. | The port reveals every cell in every mode. |
| `HouseClass::Update`, `Game::MainLoop` | `0x4F8851`, `0x55DDB9` | A game on a network that is not Cooperative advances the scenario's random generator once a particular cell has been revealed. | Not ported. |

The lobby calls the rest of Cooperative's overrides: the campaign list, the allowed countries of each campaign (`CampaignPlayerN` and `CampaignEnemyN`), the campaign difficulty, and the saved campaign progress in `COOPSAVE.INI`. Slot 26 gives the computer players the map's remaining start spots and an AI difficulty of 2 minus the campaign difficulty. A launch file supplies all of these through its seats, so the port has no code for them.

## Unholy Alliance

Slots 4 and 6 set `Bases` on. Slot 50 places every entry of `RulesClass::BaseUnit` for each house, where the base class places one. The Unholy overrides file says each player gets an Allied and a Soviet MCV. The rules' `BaseUnit` list has three entries, `AMCV,SMCV,PCV`, and a netsync run with `-GameMode 4` gives each house three units at the start, where `-GameMode 1` gives one. The vtable has no code that allies computer houses, so the mode does not ally them by itself.

The port's netsync run in `tests/netsync/unholy-mode-side-a.txt` shows the same thing: each computer house's `allies` mask holds only the Special house's bit, as it does in the Battle run.

The lobby's own team defaults are not established. Slot 12 returns 3 when `AlliesAllowed` is set, and the lobby stores that as a new computer seat's team value. `AllyTeams` (slot 34) allies houses that share a team value. Whether team 3 is a shared team for all computer seats is not established, so the port sets no team for a computer seat.

## How a mode reaches gamemd

- **The lobby.** Selecting a mode in the lobby's combo box sets `SessionClass::Config.MPModeIndex` and `MPGameMode` (`0x5E7160`). The map selection sets `Config.ScenarioIndex`.
- **The player's last choice.** The options file keeps a `GameMode` key per connection type, in sections `[Skirmish]`, `[LAN]` and `[WonlinePref]`. The reader (`0x697F10`) uses the first mode's index as the default. The startup code reads `GameMode` from `[MultiPlayer]` (`0x698413`), looks that index up, and falls back to the first mode when the index is unknown.
- **Launch files.** gamemd.exe has no reader for launch-file keys. Strings such as `IsSinglePlayer`, `UIMapName`, `HouseAllyOne` and `Multi1` are absent from the binary. The `-SPAWN` path is in the port, and the spawner that writes the file is outside this repository.
- **The CnCNet client and spawner.** Their source was read through the GitHub web pages, not cloned. The `yrpp-spawner` file `src/Spawner/Spawner.Config.cpp` reads `GameMode` from `[Settings]` into a field named `MPModeIndex`, with no range check. The key and its integer meaning therefore match this port. The client's `WriteSpawnIni` writes `UIGameMode`, a display name, and no `GameMode` line itself. Its per-mode forced options come through `GameMode.ApplySpawnIniCode` from the client's configuration, so a `GameMode` line could still arrive that way; the Yuri's Revenge entries were not in the readable repositories. Alliances come from team ids in `AllianceHolder.WriteInfoToSpawnIni`: a computer house is allied only when its team id is above zero.

## What the port does

- `code/mpmodeparse.cpp` parses `MPModesMD.ini` and an overrides file's `[MultiplayerDialogSettings]`. `tests/mpmode` covers it with written text.
- `code/mpmode.cpp` loads the table from the game's data and holds the running match's mode.
- `code/spawnerconfig.*` reads `GameMode` (default 1) and includes it in the session identity. Its bool keys match without regard to case, so the client's `Superweapons` reads as `SuperWeapons`. `SCHEMA_VERSION` is 8, because a file that read a key differently before this change describes a different match.
- `code/spawner.cpp` plays Battle, with one debug line, when `MPModesMD.ini` cannot be read or the index is not listed. It never refuses a launch for the mode. It clears `AlliesAllowed` when the mode forbids alliances, and turns `Bases` on for Unholy. A resumed game takes the mode from the file, because a save does not record one.
- `code/scenario.cpp` `Create_Units` places every base unit for Unholy.
- `code/house.cpp` `MPlayer_Defeated` applies the Cooperative winner choice, `AI` lets a person's win end a Cooperative match, and `Computer_Paranoid` treats Cooperative as allowing alliances.
- `code/scenario.cpp` `Assign_Start_Positions` and `Assign_Houses` apply Cooperative's start positions and alliances. `code/sidebar.cpp` opens the briefing, and `code/loadscreen.cpp` draws the Cooperative load screen.

## Open items

- FreeForAll's start-spot renumbering (slots 4 to 6 and 20) is not ported. A FreeForAll launch keeps the port's existing start assignment.
- Siege is not selectable, since the shipped file has no `[Siege]` section. Its overrides (slots 4 to 6, 15, 31, 33, 48, 50) are not ported.
- Team Game's `MustAlly` (slots 11 and 13) is not ported. The port has no per-seat alliance lock.
- `RandomMapsAllowed` (field 5) is stored but not used. A launch file names a fixed scenario.
- The lobby's team defaults for computer seats (slots 11 and 12) are not ported.
- Cooperative's next-mission and retry chain is not ported: slot 40 and the win and defeat paths in the table above.
- The mode's override file is read into the rules in gamemd, after `RULESMD.INI`, so a mode can change any rule; the port reads only the file's alliance flags.
- The meaning of slots 16, 38 and 47 is not identified.
- `AllyChangeAllowed` is in the Team Game file. The constructor does not read it, and its reader was not located.
- The per-mode options the client sends for Unholy Alliance and Free For All are not established. The Yuri's Revenge entries of its configuration were not in the readable repositories. The `GameMode` key and its meaning match the CnCNet spawner's reader.
- Whether a computer house starts allied in Unholy Alliance under the lobby's team defaults is not established. The port does not ally computer houses.
