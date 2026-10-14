# Testing OpenYR with the CnCNet client

This page covers how the CnCNet client starts OpenYR's `Game.exe`, what our build does with the launch file, and which lobby options and game modes testers can use. The findings come from the client's config files and log, the copied lobby files, and our own code. We have not run the client against this build.

## Summary

- The client starts the executable named by `GameExecutableNames` with `-SPAWN`, followed by `ExtraCommandLineParams`. Our `Game.exe` reads the `SPAWN.INI` the client writes and plays the match it describes.
- Our build needs no injected DLL. Ares, Phobos and the CnCNet spawner DLL are not needed for the options and modes below. [What Ares and Phobos give the setup](#what-ares-and-phobos-give-the-setup) lists each thing they provide.
- Some lobby options and modes do not work in our build. Testers avoid them; see [Lobby options](#lobby-options) and [Game modes](#game-modes).
- The main path is the CnCNet page in our main menu. It makes a separate copy of the client folder with the edits below, keeps the copy up to date, and can start the client from it. Testers do not edit INI files by hand.
- `scripts/cncnet_tester_setup.ps1` makes the same copy from a script. It is the fallback when the page cannot be used.

## Status

| Item | Status |
| --- | --- |
| Launch file reader (`SPAWN.INI`: seats, tunnel, mode index) | Built into `Game.exe`; `spawncontract` in ctest |
| CnCNet page: copy, destination, edits, marker, update, removal, start, remembered choices | Built into `Game.exe`; `cncnetcopy` in ctest and the page screenshots. Not run with the client |
| Superweapons key | Read case-insensitively from this build on, as gamemd does. See [Lobby options](#lobby-options) |
| Unknown command-line switches | Ignored by the parser. Not seen in a running game |
| The exact command line the client builds | Not verified. `client.log` does not record it |
| The client starts `Game.exe` from the copy | Not verified |
| The updater leaves the copy's edits alone | Not verified |
| The public CnCNet tunnel accepts OpenYR packets | Not verified. We have only our local relay, `scripts/cncnet_tunnel_relay.py` |
| Lobby options and map code | Checked against our key catalog; see the tables |
| Map files | Not checked. The copied client folder has no `.map` files |

## How the client starts the game

The client builds the command line in its own code, so the copied files do not show the final string. From `Resources\ClientDefinitions.ini` we expect:

```text
Game.exe -SPAWN -SYNCLOG
```

- `GameExecutableNames` names the program. Only its first entry is used.
- `ExtraCommandLineParams` follows `-SPAWN`, as its comment says.
- `GameLauncherExecutableName` is commented out, so no launcher runs first.
- `UnixGameExecutableName` applies only on Linux and macOS.

The stock config passes Syringe's `-i=` DLL options and `gamemd-spawn.exe --args="..."` through the same parameter. The `-SPAWN` inside `--args` is the switch the stock game reads. We can confirm that the client adds `-SPAWN` itself only by running it. If the game opens its main menu instead of a match, the client did not pass `-SPAWN`. If the game shows a refusal box that says `SPAWN.INI` is missing, the switch arrived and the file was not found beside the game.

### Switches the stock config passes

| Switch | Game.exe | Needed for OpenYR |
| --- | --- | --- |
| `-LOG` | Ignored | No. Use `-SYNCLOG` for the per-frame log |
| `-CD` | Ignored. We found no disc check in the code to switch off | No |
| `-Include`, `-Inheritance` | Ignored | No. No lobby file uses the include or inherit syntax |
| `-RA2ModeSaveID=` | Ignored | No. It names the saves of the stock RA2 mode |

Command-line switches are matched exactly, or by a fixed prefix, in `Parse_Command_Line` (`code/init.cpp`). A switch that matches nothing is skipped without an error. The substring checks, such as `-MPDEBUG`, do not match these strings.

### Compatibility check

The client compares `CnCNetYRLauncher.exe`, `gamemd.exe` and `gamemd-spawn.exe`. Its log also lists `Resources\clientdx.exe`. `Game.exe` is not in the stock list, so two testers on different OpenYR builds would not be refused. The setup script adds `Game.exe` to `CompatibilityCheckExecutables`. Whether the client then refuses a mismatched join is not verified.

Both testers still run the same build, as the tester guide says.

## Setting up a separate copy

### The CnCNet page

Open the main menu and choose CnCNet. The page uses the install and the copy folder it remembers. Without a remembered install, it looks for one in this order: the `installpath` value under `HKLM\SOFTWARE\Westwood\Yuri's Revenge`, which the stock installer writes; the folder that holds the running `Game.exe`; and that folder's parent. A folder counts as an install when it holds `Resources\ClientDefinitions.ini` and `CnCNetYRLauncher.exe`. Choose folder... picks another install.

The copy goes to a destination folder, shown on the page's OpenYR copy line. Without a remembered destination, the default is:

- the running build's own folder, when that folder is an OpenYR copy;
- otherwise `<install> - OpenYR` beside the install, when the install's parent folder can be written;
- otherwise `OpenYR CnCNet` in a `Games` folder on the drive Windows is on.

Testers extract the package inside their Yuri's Revenge folder, so the running `Game.exe` is often inside the install. The default never puts a new copy in the running build's folder, because the copy would then sit inside the install.

Change destination... picks another folder. The folder must be empty, or hold an OpenYR copy of the same install. The page never asks for administrator rights, so a folder it cannot write to is refused and the page says so.

The page keeps the install and the destination in the `[CnCNet]` section of the settings file, which is `RA2MD.INI` unless `OPENTS.INI` names another. It saves them when you choose either folder, when Find install finds one, and when a copy starts. Next time the page opens, it uses them. A remembered install that is no longer an install is replaced by a search, as above.

Before it changes anything, the page lists the copy's folder, every file it replaces or adds, and each edit. The plan is refused, and Create copy stays disabled, when:

- the destination is the install folder or lies inside it;
- the install lies inside the destination, so the copy would contain itself;
- the destination cannot be written. The page creates and deletes a test file in the nearest folder that exists;
- the destination exists, is not empty, and is not an OpenYR copy;
- the destination is an OpenYR copy made from another install;
- the drive has less free space than the install needs plus 1 GB.

Create copy starts the copy on a worker thread, so the menus keep drawing. The progress bar counts bytes. Cancel copy stops the copy and removes the files that run created. The install is only read.

The page reports three things about the copy:

- whether the copy exists and where it is;
- whether its edited files still hold the values the copy needs;
- whether its `Game.exe` is the same file as this build's.

When the client update has undone the edits, the button reads Repair copy. Repair and Update copy run the same job: they copy `Game.exe`, `Language.dll` and the `ui` folder again and make the edits again. They do not copy the install again.

Remove OpenYR copy asks for confirmation. It deletes only a folder that holds the marker file. Start CnCNet runs `CnCNetYRLauncher.exe` from the copy and then ends this game, as Exit Game does.

The page's screenshots come from a fake install. `tests/autotest/cncnet-page-fixture.ps1` builds it from the text files of a client folder. Its executables are placeholder bytes that nothing runs. The `cncnet-page-*.txt` scripts run the page against that install, with the settings the fixture prints.

### The script

Double-click `CnCNet setup.bat` in the unpacked OpenYR folder, which holds `Game.exe` and `Language.dll`. It asks whether to make a copy or remove one.
- Making a copy finds the CnCNet install or asks for it with a folder picker, shows where the copy will go, and asks before it copies anything. It can also add an "OpenYR CnCNet" desktop shortcut to the copy's `CnCNetYRLauncher.exe`.
- Removing one asks for the copy folder and confirms before deleting it.

`CnCNet setup.bat` is `scripts/cncnet_tester_setup.ps1` behind a short batch header, so one file can be double-clicked; Windows opens a `.ps1` in Notepad instead. `scripts/make_cncnet_setup_bat.ps1` writes it, and the package script runs it.

The script copies the install and makes every change in the copy. The original install is not touched. The page makes the same changes and writes the same marker file.

To run it without questions, pass the install folder:

```powershell
powershell.exe -NoProfile -ExecutionPolicy Bypass -File cncnet_tester_setup.ps1 -InstallPath "C:\Games\Command & Conquer Red Alert II"
```

The install it finds first is the OpenYR folder itself, when OpenYR was unpacked into a CnCNet install, then the install the game's registry keys name, then the usual Steam, Origin and EA folders. The copy goes beside the install, named with ` - OpenYR` added, when Windows lets the script write there; otherwise it goes to `Games\OpenYR CnCNet` on the system drive. Pass `-Destination` to choose another empty folder. The script refuses a destination inside the install or the OpenYR folder, and one it cannot write to without administrator rights.

The script prints each change as it makes it:

| File in the copy | Change | Why |
| --- | --- | --- |
| `game.exe` | Replaced by `Game.exe`; the stock file is kept as `game.exe.cncnet-orig` | The client starts the game from `GameExecutableNames` |
| `Language.dll`, `ui\` | Added from the build | The engine needs them |
| `Resources\ClientDefinitions.ini` | `GameExecutableNames=Game.exe`, `ExtraCommandLineParams=-SYNCLOG`, `Game.exe` added to `CompatibilityCheckExecutables`; the original is kept | Starts OpenYR and adds it to the compatibility check |
| `OPENTS.INI` | New file: `[Files]` `Settings=OPENYR.INI` | Keeps our settings out of `RA2MD.INI`, which the client also uses |
| `updateconfig.ini`, `Resources\Config\UpdaterConfig_*.ini` | Update mirror set to `https://127.0.0.1:9/`; originals kept | See [Updater](#updater) |
| `Resources\ClientDefinitions.ini` | Made read-only | See [Updater](#updater) |
| `OPENYR-TESTER-COPY.txt` | New file listing every change | Undo and reports |

Options:

- `-KeepUpdater` leaves the update mirrors and the read-only flag alone.
- `-SkipCompatibilityEntry` leaves `CompatibilityCheckExecutables` alone.

To undo, run the script with `-Remove -Destination "<the new folder>"`. It deletes only a folder that holds `OPENYR-TESTER-COPY.txt`, and nothing else.

The script refuses to run when the destination exists and is not empty, when it is inside the install, or when the install lacks `CnCNetYRLauncher.exe`, `ra2md.mix` or `Resources\ClientDefinitions.ini`.

We tested the script against a scratch copy with the text files of the client folder and zero-byte stand-ins for three binaries. The install's 302 file hashes matched before and after every run. We have not run it against the full 2.5 GB install, and we have not started the client from its output.

## Updater

The client checks its installed files at start. The `version` file lists each installed file with a hash, including `Resources/ClientDefinitions.ini`, `INI/MPMaps.ini` and the lobby configs. An edited `ClientDefinitions.ini` no longer matches its entry, so an update would write the stock copy back. `Game.exe` is not in the list.

The update channel decides which mirror the client uses. `Resources\OptionsWindow.ini` maps each channel to a file in `Resources\Config\`, and the client copies that file over `updateconfig.ini` (`OverwriteOnMismatch`). The setup script therefore changes all four channel files and `updateconfig.ini`, and marks `ClientDefinitions.ini` read-only.

We did not find an update-off setting in the copied config files.

Tester advice: after the first start, and after any client update, check that `ClientDefinitions.ini` still has the three edited lines. If it does not, open the CnCNet page and choose Repair copy. The script can also make a new folder. If the client reports an update error at start, send us the message and the `OPENYR-TESTER-COPY.txt` file.

## Lobby options

The lobby writes spawn keys through `SpawnIniOption` in `Resources\GameLobbyBase.ini`, and merges map code through `CustomIniPath` or `DataWriteMode=MapCode`. "Read" below means our spawner or rules loader has a reader for the key. For merged files, "catalogued" means every key in the file has a reader in our key catalog (`manual/data/ini-keys.yaml`).

| Option | Written as | Status | Advice |
| --- | --- | --- | --- |
| Short Game | `ShortGame` | Read | Either |
| MCV Repacks | `MCVRedeploy` | Read | Either |
| Multi Engineer | `MultiEngineer` | Read | Either |
| Ingame Allying | `AlliesAllowed` | Read | Either |
| Destroyable Bridges | `BridgeDestroy` | Read | Either |
| Build By Ally | `BuildOffAlly` | Read | Either |
| Starting Credits | `Credits` | Read | Either |
| Starting Units | `UnitCount` | Read | Either |
| Game Speed | `GameSpeed` (index) | Read | Either |
| Crates Appear | `Crates` | Read | Either |
| Auto Save | `AutoSaveInterval`, 7200 or 0 | Read from this build on | Either. 7200 frames is two minutes at 60 FPS |
| Red Alert 2 | `Ra2Mode` | Not read | Leave off. The match plays YR rules |
| Lock Game Speed | `DisableGameSpeed` | Not read | Leave off. Players can still change speed |
| Blind Start | Blanks `[Header]` keys in the map | Not read; no effect | Either |
| YR Rebalance | Merges `Yuri Rebalance Patch.ini` | Catalogued except `FallingDamageMultiplier` | Leave off until checked |
| No Yuri, No France, No Spy, No Stolen Tech, Engi Dog Immune | Merges a rules file | Catalogued | Either |
| Super weapons: All, Strategic, Tactical, None | Merges `SuperWeapons\*.ini` | Catalogued | Either. See the forced keys below |
| Crate Frequency: Minimal, Increased, Extreme | Merges `Crates\*.ini` | Catalogued | Either, with Crates on |
| Crate Frequency: Normal (the default) | `Crates\Normal.ini` | The file is not in the install | Leave Crates off |
| AI Modifier: Vanilla | `AI\No Change.ini`, empty | Read | Use |
| AI Modifier: Brutal | `AI\Brutal AI.ini` | Catalogued | Use |
| AI Modifier: Extreme AI, Nightmare AI (Beta) | Ares-era files | 289 and 324 key names not read | Avoid. Their headers say "Ares and Phobos" |

### Forced keys

The lobby writes these keys for every match. They come from `Resources\GameOptions.ini`, `ForcedSpawnIniOptions`, and from each mode's forced options in `INI\MPMapsBase.ini`.

| Key | Written | Status |
| --- | --- | --- |
| `Bases` | `Yes` | Read |
| `FogOfWar` | `No` | Read |
| `ReconnectTimeout` | `1400` | Read |
| `GameMode` | `1` to `9`, by mode | Read. See [Game modes](#game-modes) |
| `Superweapons` | `False` | Read as `SuperWeapons`. The spawner matches keys without regard to case, as gamemd does, so the value is read. See the paragraph below |
| `AINamesByDifficulty` | `Yes` | Not read. We read `DifficultyBasedAINames`, so computer players keep the name `Computer` |
| `SidebarHack` | `Yes` | Not read |
| `Protocol`, `FrameSendRate` | `0`, `2` | Not read, by design. See [the launch file page](../manual/content/formats/spawn-ini.md) |
| `Observer.ShowAIOnSidebar`, `SpecialHouseIsAlly`, `DefeatedBecomesObserver` | `Yes`, `No`, `Yes` | Not read. Their effect in the client is not known to us |

The client forces `Superweapons=False` into every match (`[ForcedSpawnIniOptions]` in `Resources/GameOptions.ini`). That is by design: the key withholds only superweapons whose type has `DisableableFromShell=yes`, and the lobby's Super weapons drop-down merges one of `INI/Game Options/SuperWeapons/*.ini` into the map, All, Offensive and Defensive set `DisableableFromShell=no` on the superweapons they keep, so those stay available; Disabled instead makes the superweapon buildings unbuildable. Before this change our spawner matched the key case-sensitively and ignored it, so superweapons left at their rules default of `DisableableFromShell=yes` stayed on when the lobby meant them off.

## Game modes

The lobby's `GameMode` value is the mode's index in `MPModesMD.ini`. The table gives the stock index names from [Multiplayer game modes](MULTIPLAYER_MODES.md).

| Lobby mode | `GameMode` | Map code | Status | Advice |
| --- | --- | --- | --- | --- |
| Battle | 1 | `Standard.ini` (empty) | Plays as Battle | Use |
| Free For All | 2 | `Free For All.ini` (empty) | Forbids alliances. The start-spot renumbering is not ported | Use with care |
| Cooperative | 3 | `Cooperative.ini` (empty) | Allies win together. The hostile-pair check is not ported | Use with care |
| Unholy Alliance | 4 | `Unholy Alliance.ini` | Turns bases on and gives each house every base unit. Whether computer houses start allied is not established | Use with care |
| Megawealth | 5 | `Megawealth.ini` | Plays as a ManBattle mode | Use |
| Land Rush | 6 | `Land Rush.ini` | The client's `MPMaps.ini` forces `GameMode=6` for Land Rush. The stock table names index 6 Duel, so the port plays that slot (ManBattle, `MPDuelMD.ini` overrides). `Land Rush.ini` lists which units may start in multiplayer. Not played with this build yet | Avoid until one test game plays as expected |
| Meat Grinder | 7 | `Meat Grinder.ini` | Plays as a ManBattle mode | Use |
| Naval War | 8 | `Naval War.ini` | Plays as a ManBattle mode | Use |
| Team Alliance | 9 | `Team Alliance.ini` | Plays as TeamGame. `MustAlly` is not ported | Use with care |
| Blitz, Blitz 2v2 | 1 | `Blitz_2v2.ini` | Four keys are not read: `UnitCostBonus`, `UnitCrateVehicleCap`, `ImmuneToRadBeam`, `AmbientSound` | Avoid |
| Mod Maps | 1 | `Standard.ini` | Map sets added by mods; not checked | Avoid |
| RA2 Ladder, RA2 Pro 2v2, YR Ladder | 1 | Ladder files | RA2 mode and ladder rules | Avoid |
| Oil In Centre | 1 | `Standard.ini` | A map variant with one option forced | Use with stock maps |
| Survival | not written | `Survival.ini` (empty) | No section in `MPMaps.ini` or `MPMapsBase.ini` | Avoid |

The `Team Alliance` map code also writes `[Settings] GameMode=4` into the map. Our loader does not read that key from the map, so the lobby's `GameMode=9` decides the mode.

## Maps

The copied client folder has no `.map` files, so we have not checked any map. Testers pick maps from the lobby's Battle list. Avoid Blitz maps, mod maps and ladder maps. CnCNet's additions in `Maps\Yuri's Revenge\` are not checked either.

## What Ares and Phobos give the setup

Each row says whether `Game.exe` already does the job, could do it natively, or does not need it. "Size" is our rough estimate of the native work.

| Feature | Provided by | Used by the lobby options above | Game.exe | Native size |
| --- | --- | --- | --- | --- |
| Starting the game through a loader (`-i=` DLL options, `gamemd-spawn.exe --args=`) | Syringe, `gamemd-spawn.exe` | Yes, in the stock config | Not needed. `Game.exe` is the executable | None |
| The `-SPAWN` start | `gamemd-spawn.exe` | Yes | Native: `Spawner_Request` in `code/init.cpp` | None |
| Launch file reader: seats, options, GameMode, saves, waits | `CnCNet-Spawner.dll` | Yes | Native: `code/spawnerconfig.cpp` and `code/spawner.cpp` | None for the keys above |
| Tunnel: `[Tunnel]`, tunnel numbers | `CnCNet-Spawner.dll` and the CnCNet tunnel servers | Online lobbies | Client side is native (`Ipx.Configure_Tunnel`). The wire format against the public servers is not verified | Verification first. Protocol work if it differs |
| Direct peers: `Ip`, `Port`, LAN | `CnCNet-Spawner.dll` | LAN lobbies | Native | None |
| Mode table: `GameMode` to `MPModesMD.ini` | The game | Yes | Native: `code/mpmode.cpp` | None, apart from Land Rush and the gaps in the table |
| INI `#include` and `$Inherits` | Ares, switched on by `-Include` and `-Inheritance` | No copied lobby file uses the syntax | Not native. The names suggest these switches enable include and inherit | Medium: the INI reader and its tests. Not needed for this setup |
| Ares-era AI modifiers | Ares and Phobos AI keys | Only the two Extreme AI options | Not native. 289 and 324 key names not read | Large. Not needed if testers avoid those options |
| RA2 mode (`Ra2Mode`, RA2 ladder maps, `RA2ModeSaveID`) | Ares and the client | Red Alert 2 option and RA2 ladder maps | Not native | Large. Not needed |
| Phobos rule key `FallingDamageMultiplier` | Phobos | YR Rebalance option | Not read | Small: one damage multiplier |
| Blitz keys (`UnitCostBonus`, `UnitCrateVehicleCap`, `ImmuneToRadBeam`, `AmbientSound`) | Ares and Phobos | Blitz maps | Not read | Small to medium per key. Not needed |
| `SFJ.ini` keys (79 names) | Ares and Phobos | No lobby mode in our copy refers to `SFJ.ini` | Not read | Medium to large. Not needed |
| `-LOG`, `-CD`, `-RA2ModeSaveID` | Ares, `gamemd-spawn.exe` | Launch line only | Ignored | None |
| `ddraw.dll` wrapper in the folder | The client (cnc-ddraw) | Launch line only | Our build uses DirectDraw surfaces (`code/dsurface.cpp`), so the wrapper sits in front of it | None. Compare the picture with the stock install |
| Phobos and Ares links and version labels | The client's lobby | Lobby buttons | Not needed | None |

## Upstream route

If the CnCNet client were to run OpenYR as its own game type, it would need these changes. We describe them here only. Any contact with the CnCNet maintainers is a decision for us to make first.

- A `ClientGameType` for OpenYR, alongside `Ares`. The type would select its own option list, with the RA2 mode, Ares-era AI and ladder maps left out.
- `GameExecutableNames=Game.exe` and `Game.exe` in `CompatibilityCheckExecutables` as stock entries, so two builds are compared by the client itself.
- Its own update channel, so an OpenYR install is not overwritten by the YR channel.
- Its own presence identifier in place of `CnCNetLiveStatusIdentifier=cncnet5_yr`, so OpenYR lobbies are not listed with stock YR games.
- Tunnel acceptance for the OpenYR packets, which needs a test with the CnCNet side.

Our `SPAWN.INI` reader already reads the file the client writes for a stock match, so the launch path needs no change on our side.

## Turn on the sync log

Add `-SYNCLOG` to `ExtraCommandLineParams` on every machine in the match, as the setup script does:

```ini title="ClientDefinitions.ini"
[Settings]
ExtraCommandLineParams=-SYNCLOG
```

Each game then writes one line per frame to its debug log, for example `SYNC frame 400 crc 6134918a`. The two machines' lines match for as long as they stay in sync. The log grows quickly, so turn it on only for test matches.

If you start the game yourself with `-USERDIR`, you can set `SyncLog=yes` under `[Options]` in that folder's `RA2MD.INI` instead.

## Where the logs go

The game writes its logs to a `Debug` folder beside `Game.exe`:

- `DEBUG_<date>_<time>.LOG` is the debug log of one game.
- `SYNC_H<heap>_<date>_<time>_F<frame>.LOG` is the out-of-sync report, written when a game goes out of sync.

The game deletes debug logs older than 14 days when it starts, so send the files soon after a test.

## After a desync

Send us, from every machine in the match:

- the `DEBUG_` and `SYNC_` files from the `Debug` folder for that session;
- the time the desync happened, the map name and the number of computer players.

## Open questions

- Does the client always add `-SPAWN`? The first game answers it.
- Check in one test match that the Super weapons drop-down's All and Disabled choices behave as described above.
- Does a Land Rush match on index 6 play as the client's Land Rush? The index is the one the client sends; the rules behind it are not checked.
- Does the public CnCNet tunnel accept our packets?
- Does the client start cleanly with the update mirrors and the read-only file in place?
- Does the client refuse a join with a different `Game.exe`, now that it is in the compatibility list?
- What do `SpecialHouseIsAlly`, `DefeatedBecomesObserver` and `Observer.ShowAIOnSidebar` change in the client?
- Which maps are safe? The copied client folder has no `.map` files, so the map check is still open.
