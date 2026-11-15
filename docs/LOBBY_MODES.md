# Lobby game modes and game options

A mod can give our skirmish lobby its own game modes and game options. We read them from the files of the CnCNet
client the mod is played through, so a mod such as Mental Omega needs no files written for us. A game in which no mod
declares any keeps the lobby exactly as it was.

## The files

`mod.ini` names the client's two files under `[Lobby]` (see the manual's `mod.ini` page):

| File | What we read |
| --- | --- |
| `Maps=`, the map list (`MentalOmegaMaps.ini`) | `[GameModes]`: the mode names, in order. One section per mode: `UIName`, `MapCodeIniName` (or `MapCodeININame`), `ForcedOptions`, `ForcedSpawnIniOptions`, `MultiplayerOnly`, `HumanPlayersOnly`. `[MultiMaps]` and one section per map: `Description`, `GameModes`, `ForcedOptions`, `ForcedSpawnIniOptions`, `Credits`, `UnitCount`, `Bases`. |
| `Options=`, the game options (`GameOptions.ini`) | `[SkirmishLobby]` `CheckBoxes` and `DropDowns`, one section per control, and `[ForcedSpawnIniOptions]`. A check box: `Text`, `ToolTip`, `Checked`, `SpawnIniOption`, `CustomIniPath`, `Reversed`, `EnabledSpawnIniValue`, `DisabledSpawnIniValue`, `Visible`. A drop-down: `OptionName`, `ToolTip`, `Items`, `ItemLabels`, `DefaultIndex`, `DataWriteMode`, `SpawnIniOption`, `Visible`. |

Every path in the files, such as `INI\Game Options\Stolen Tech.ini`, is taken from the mod's folder.
`scripts/setup_mo_mod.ps1` copies Mental Omega's files into the mod folder in their own folders and writes the
`[Lobby]` section, with `CustomMapModes=` taken from the client's `AllowedCustomGameModes`.

## How a choice becomes a match

We follow the open-source CnCNet client (xna-cncnet-client): `GameMode.cs`, `GameLobbyBase.cs`,
`GameSessionCheckBox.cs`, `GameSessionDropDown.cs`, `MapCodeHelper.cs` and `Map.cs`, read for their behaviour only.

- **The mode's overlay** is `INI\Map Code\<MapCodeIniName>`, by default `INI\Map Code\<mode name>.ini`. The mode
  section's `CustomIniPath` is not read by the client, and we do not read it either.
- **Forced options** come from the section `ForcedOptions=` names: a number sets a drop-down's item, anything else a
  check box. The mode's apply first, then the map's (`ForcedOptions=` on a map lists several sections). A forced control
  shows its value and cannot be changed; the person's own value comes back when no mode or map forces it.
- **Launch keys**, in the client's order: each check box (`EnabledSpawnIniValue`, by default `True`, when checked,
  reversed by `Reversed=yes`), each drop-down (`Boolean` writes whether the item is past the first, `Index` its
  position, `String` the item, `MapCode` nothing), then `[ForcedSpawnIniOptions]`, the mode's
  `<mode>ForcedSpawnIniOptions` section (or the one its `ForcedSpawnIniOptions=` names), the map's forced keys and its
  `Credits`, `UnitCount` and `Bases`. A later key replaces an earlier one.
- **Overlays**, in the client's order, over the map: the mode's overlay, `INI\Map Code\GlobalCode.ini` when present,
  each check box's `CustomIniPath` when it is on (off when `Reversed`), then each `MapCode` drop-down's item. Each
  overlay is merged key by key; an empty value removes the map's entry. Its `[GameModeIncludes]` entry for the mode
  merges one more file. Its `ReplaceMap<Section>` sections (`Aircraft`, `Infantry`, `Units`, `Structures`, `Terrain`)
  swap the type of the map's objects of a type, or remove them when the value is empty.

The client writes the merged map as `spawnmap.ini`. We merge into the scenario as the game reads it, before anything
in it is read, so the overlays' rules sections reach the rules as a map's own sections do.

The launch keys set the match options the way a launch file does: the menu's own switches and sliders are not used,
and an option no key sets takes the launch file default. `GameMode` picks the multiplayer mode, so Unholy Alliance and
Ultimate Alliance play the engine's Unholy mode. A key the game does not read, such as `StolenTech`, is written to the
debug log once a run and kept in the launch file the game writes.

## Persistence

The choice is the mode and the person's value for each option, as `Mode|option=value|...`. The lobby remembers it in
`[Skirmish] Lobby=` of the settings file, the session keeps it in `GameOptionsType::LobbyChoice`, a save stores it,
and `Spawner_Describe_Session` writes it as `[LobbyChoice]` with the launch keys in `[Settings]`, so a replay or a
restart applies the same overlays. The choice is part of the session identity only when a match has one.

## The skirmish screen

The lobby lists the declared modes in place of its sliders, apart from `MultiplayerOnly` modes, and the options as
rows of the declared settings pattern (`code/ui/uisettingrows.*`, `ui/setting-rows.rml`) in place of its switches.
A drop-down is the `CHOICE` row kind. A forced row is greyed and says "Set by <mode>" or "Set by the map" next to it,
and its tip says the same. The map picker lists the maps the mode offers: those whose `GameModes` name it, and the maps
the list does not name when the mode is in `CustomMapModes`. Choosing a mode the shown map is not in moves the lobby
to the first map on hand that the mode offers.

## Not done yet

- The LAN lobby and the built-in CnCNet lobby, which need to send the host's choice to the other machines.
- Co-op and challenge rules: the map's co-op houses, human-only seats, `CoopDifficultyLevel`, `MinPlayersOverride`,
  `EnforceMaxPlayers`, `ForceNoTeams` and `ForceRandomStartLocations`. A co-op mode's overlays apply when a launch file
  names it, but the seats are not arranged as the client arranges them.
- Randomized map code (`RandomizedMapCodeIniNames`), `MultiplayerGlobalCode.ini`, the difficulty drop-down's overlays,
  and options that disable sides (`DisallowedSideIndices`).
