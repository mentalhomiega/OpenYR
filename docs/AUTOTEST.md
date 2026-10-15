# Unattended test runs

`-AUTOTEST=<script>` runs a game that tests itself. The window stays hidden, the game behaves as
though it has the focus, and it ignores the real mouse and keyboard: the cursor rests at (200, 200)
with no button pressed, and it is never captured, confined or moved. A crash saves its report
without showing the crash dialog. A test can therefore run while someone uses the computer.

Combine it with `-SPAWN` to start a skirmish straight away. `scripts/run_autotest.ps1` in the
workspace that hosts this repository writes a muted settings file and a `SPAWN.INI`, then starts
the game this way.

## The script

Each line is `<frame> <command> [arguments]`. Blank lines and lines starting with `;` are ignored.
A command runs once the game frame reaches its frame, in file order.

| Command | Effect |
| --- | --- |
| `command <Name>` | Runs a registered command, such as `CenterBase`, `DeployObject` or `ScreenCapture` |
| `hotkey <key>` | Writes the name of the command that key is bound to, or `none`. A letter or a single digit is read as that character; any other number is a virtual-key code, plus 256 for Shift, 512 for Control and 1024 for Alt |
| `chrono <frames>` | Starts the chronoshift screen effect for that many frames per swirl, as the Chrono Screen Effect trigger action does |
| `select <TypeID>` | Selects every object of that type the player controls: the player's house, and any house the map marks `PlayerControl=yes` |
| `produce <TypeID>` | Starts building that type, as a click on its cameo does. A type the house may not build has no cameo, so it is logged and not started |
| `place <TypeID>` | Places the finished structure on the first legal cell found around the player's construction yard; a finished vehicle, aircraft or infantryman is released from its factory as the sidebar does |
| `grantunit <TypeID> x y` | Like `spawn`, but the object belongs to the player, so `move` and `strike` order it |
| `move <TypeID> <x> <y>` | Orders the player-controlled units, infantry and aircraft of that type (see `select`) to the cell; structures are not moved |
| `stop <TypeID>` | Sends the Stop command's idle event to the player's objects of that type that can move or fire. It logs how many it sent |
| `attack <TypeID>` | Orders the player-controlled objects of that type (see `select`) to attack the nearest structure of another house that has a construction yard |
| `view <x> <y>` | Centres the view on the cell |
| `follow <TypeID>` | Keeps the view centred on one of the player's objects of that type, until the next `view` |
| `record <frames>` | Saves a screenshot every that many frames; `record 0` stops |
| `dump` | Writes the player's credits, structures and units, with their missions and movement, to the debug log |
| `enemies` | Writes the other houses' structures and their unit and infantry counts to the debug log |
| `iq` | Writes each house's base IQ (the map's `IQ=`, 0 for a skirmish computer), its effective IQ, and whether each [IQ] gate is open for it |
| `owners <TypeID>` | Writes the type's owner bits and each house's country bit to the debug log |
| `canbuild <TypeID>` | Writes each house's own and acted country, the type's `RequiredHouses` and `ForbiddenHouses`, and whether `Can_Build` allows the type for that house now |
| `anims` | Writes the first entries of the animation list to the debug log |
| `truecolour <NAME.SHP>` | Writes whether a PNG replaces that shape (see [TRUECOLOUR.md](TRUECOLOUR.md)), whether the shape is an SHP (`shp`) or was made from a sheet that has none (`png-only`), with the shape's frame count and size, the sheet's size, the frames it covers, whether it has a house-colour mask, whether the shadows come from the PNG or the SHP, and the centre pixel of its first frame |
| `truecolourdir <path>` | Adds a directory to the places the game looks for files, so a test can supply PNG sprites without copying them into the game directory; a PNG not found before is looked for again |
| `exportshape <NAME.SHP> <out.png>` | Writes every frame of that shape to a PNG sheet in the layout [TRUECOLOUR.md](TRUECOLOUR.md) reads, coloured with the current theater's unit palette, with index 0 transparent; when the shape has house-colour pixels, also writes `<out>_hc.png`, the matching house-colour mask. The output path is the rest of the line |
| `loadshot <name>` | Saves what the window shows each time a loading screen is complete, as `<name>-<count>.tga` in the `Screenshots` folder. It takes effect when the script is read, before the first game frame, whatever frame the line names |
| `triggers [all]` | Writes the trigger types with their owner, the events they wait for and whether a live trigger of each is enabled (only the enabled ones, unless `all`), then every tag with the objects and cells it rides on, and the local and global variables that are set |
| `typecounts` | Writes how many structure, vehicle, soldier and aircraft types there are, and any whose ID is not a plain name, which is how a list read as one name shows up |
| `where <TypeID>` | Writes each object of that type, whoever owns it, with its cell, mission, destination, target and strength; a soldier also gets a line with its action and whether it is dug in |
| `animlog <TypeID>` | Writes each object of that type, whoever owns it, with its walk counter and the body voxel frame that counter selects, its turret frame and facings, whether its locomotor moves, its target, and whether a DiskLaser ring is active |
| `forceattack <TypeID> x y` | Orders the player's objects of that type to force-attack the object on that cell, or the ground there, as a Ctrl+click does. Autotests never report Ctrl as held, so the order is given directly |
| `quantity <TypeID>` | Writes how many of that type each house is counted as having, owned and active, which are the two counts the map events read. A structure uses the building counts, a unit, infantry or aircraft type its own |
| `where <TypeID>` | Writes each object of that type, whoever owns it, with its cell, mission, destination, target and strength; a soldier also gets a line with its action and whether it is dug in; a teleporting object gets its warp phase (0 idle, 1 warping out, 2 holding), the frame the phase ends, whether it is warping, whether a chrono warp holds it, and whether it is selected |
| `quantity <StructureID>` | Writes how many of that structure each house is counted as having, owned and active, which are the two counts the map events read |
| `killhouse <House> [kind]` | Destroys the buildings (kind 1), the vehicles, soldiers and aircraft (kind 2) or everything (kind 0, the default) the house owns, with one of the player's objects as the attacker. An underscore in the name stands for a space |
| `killtag <Tag>` | Destroys every object that carries a tag of that ID or name, with one of the player's objects as the attacker |
| `transfer <TypeID>` | Gives the first live object of that type that a computer house owns to the player through `TechnoClass::Set_Owning_House`, the path a mind control capture takes |
| `primary <TypeID>` | Toggles the primary flag of the player's first structure of that type, as the structure's self action does when the house has more than one factory of its kind |
| `factories` | Writes each structure that builds something, whoever owns it, with what it builds (`vehicle`, `infantry`, `aircraft` or `structure`), whether it is naval, whether it is its house's primary factory and whether it is in limbo |
| `kill <TypeID> [1]` | Destroys every computer object of the type with no attacker, the C4 warhead forced; with a `1`, every player object of the type instead |
| `hollow <TypeID>` | Sets the strength of the player's factory-held object of that type to zero while it is in production, so it leaves the factory with no radio link (the state of an internal dummy unit). It logs each object it sets. |
| `setlocal <index> <value>`, `setglobal <index> <value>` | Sets a scenario variable when the value is 1 and clears it when it is 0, as a trigger action does |
| `dumpfile <name>` | Copies the file the game finds under that name, from a MIX or the game folder, to `dump-<name>` in the working directory, and logs its size |
| `trigaction <number>[@<House>] <value> <waypoint>` | Carries out the trigger action with that number for the player's house, or for the named house with an underscore in place of a space, as a trigger would. The value fills the action's number, house or time field and the waypoint its waypoint field. The log gets `done` or `failed` |
| `hash [frames]` | Writes a hash of the game state to the debug log, and again every that many frames when given; `hash 0` logs once and stops the repeats |
| `speed <TypeID> [interval]` | Writes the current and target speed of the player's objects of that type that fly or jump, with their cell, height, mission and destination. A flying object's speed is a fraction of its full speed; a jump-jet object's is in leptons per frame. Repeats every interval frames when given |
| `sell <x> <y>` | Starts selling the player's structure on that cell, as the sell cursor's click does |
| `infiltrate barracks\|warfactory` | Marks the player's house as having spied on that building, so its new trainable infantry or units start as veterans |
| `occupy <TypeID> x y` | Puts the player's first infantry of that type inside the structure on that cell, without walking there |
| `radar <TypeID>` | Writes each of the player's objects of that type with whether it is in limbo, whether the radar tracks it, whether the player has discovered it, and its radar pixel |
| `neutral <TypeID> x y` | Puts an object of the type on that cell, owned by the house of the Civilian side, as `own` does for the player |
| `rank <TypeID> <0\|1\|2>` | Makes every object of that type, whoever owns it, rookie, veteran or elite |
| `veterancy <TypeID>` | Writes the rules' `VeteranRatio` and `VeteranCap`, the player's score, then each object of that type with its rank, experience and cost |
| `crate <Powerup> x y` | Puts a crate that holds that powerup, named as in the crate list (`money`, `unit`, `heal`, `cloak`, `explosion`, `napalm`, `squad`, `darkness`, `reveal`, `armor`, `speed`, `firepower`, `icbm`, `invuln`, `veteran`, `ion`, `gas`, `tiberium` or `pod`), on the nearest free cell to the cell |
| `warpout <TypeID> <0|1>` | Sets or clears the warping-out flag of the player's objects of that type, without a warp effect |
| `unload <x> <y>` | Starts the unload mission of the player's structure on that cell. A vehicle on a cell with no structure unloads its passengers instead |
| `rally <TypeID> x y` | Sets the rally point of the player's structures of that type to the cell, without the nearby-cell search an Alt-click makes |
| `census cells` | Writes the eight tiberium cells nearest the player's first building, nearest first, with their squared distance in cells |
| `census ore` | Writes, for each tiberium type, the cells holding it with their total density and credit value, how many of them can still grow or spread, and the lengths of its growth and spread queues. `census` writes the houses instead: each house's money, power, buildings, units, infantry and aircraft, and for each computer house its difficulty, the resource gatherers and destinations it owns (slave miners included), the factories it has by type, and the units, infantry, aircraft and structures it has finished producing so far |
| `buildtrace <0\|1>` | Logs, as BUILDTRACE lines, each structure a computer house starts building from its base plan and the cell where it places a finished one |
| `rallyclick <TypeID> x y` | Gives the player's structures of that type the rally click of an Alt-click on the ground, with the nearby-cell search |
| `rallyof <TypeID>` | Writes the rally point (archive target) of each of the player's structures of that type, as the cell it names, or `-1,-1` when none is set |
| `pickable <TypeID>` | Writes each object of that type the player owns, with whether the clickable list holds it and, when it does, the window pixel its entry is drawn at (`window x,y`, or `-1,-1` when it is not listed). The list is the one the last render built |
| `pickat x y` | Writes the object a left click at that window pixel would pick from the clickable list and the cell under it, without selecting anything |
| `cellpixel x y` | Writes the window pixel of the centre of that cell under the current view, for a `windowclick` step |
| `beacon place x y` | Queues a beacon for the player on that cell, as a click in beacon mode does. In a network game the other copies place it too |
| `beacon select x y` | Selects the beacon within 128 leptons of that cell's centre, or writes that none is there. Clears the unit selection |
| `beacon delete` | Queues the removal of the selected beacon when the player owns it, as the Delete command does |
| `beacon list` | Writes each beacon with its house, slot, cell, whether the player sees it, whether it is selected and its text |
| `beacontext <text>` | Queues the text for the player's selected beacon, as a message typed on a selected beacon does |
| `defeat <house>` | Runs the defeat a house gets once it has nothing left, on that house: its forces are destroyed, then MPlayer_Defeated runs. The house is named as the waits name one; `player` is the local player. Every copy of a network game must run it on the same frame |
| `observer` | Writes the local player's defeat, watching, whole-map and Session.ObiWan state, its owned objects, the houses still playing, its selection and the beacon and planning modes |
| `modes` | Writes the repair, sell, power, waypoint and beacon placement flags and the targeting flag as 0 or 1, then the object pending placement |
| `cell <x> <y>` | Writes the cell's map state, then its building and its occupier |
| `log <text>` | Writes the whole text to the debug log, as `AUTOTEST frame N: log <text>` |
| `rule <Name> <value>` | Changes a rule for the rest of the run: `CanDetonateTimeBomb` (0 or 1) and `BallisticScatter` (leptons) |
| `strike <TypeID> x y` | The player's objects of that type attack the object on that cell |
| `strikeobj <TypeID> x y [radius] [land\|naval\|all]` | The player's armed units and infantry attack the nearest live object of that type within `radius` cells (default 3) of the cell. The target is chosen when the line runs, so a target that has moved is still found; the units keep it until it dies or leaves the map. `land` orders only units that are not ships, `naval` only ships, and `all` (the default) every armed unit |
| `treehit <Warhead> x y` | Hits the terrain object on that cell with 100 points of damage from the warhead, with no firer, and writes its strength before and after |
| `trees` | Writes every terrain object on the map with its cell, strength, armor and whether it is immune |
| `waitcell <TypeID> <x> <y> [radius] [timeout]` | Pauses the script until one of the player's objects of that type is within `radius` cells of the cell (default 2), measured as a straight line. Objects not on the map do not count. See [Waiting for conditions](#waiting-for-conditions) |
| `waittrigger <TriggerID or name> [timeout]` | Pauses the script until that trigger has sprung at least once. `runtrigger` and `trigaction` carry out actions without a spring, so they do not count |
| `waitowns <House> <TypeID> <count> [timeout]` | Pauses the script until at least `count` objects of the type that the house owns are on the map. A structure still in production does not count, and neither does an object in limbo. `player` names the player, and an underscore stands for a space |
| `waitlocal <index> <0\|1> [timeout]` | Pauses the script until that scenario local variable has that value |
| `triggerlog <0\|1>` | With `1`, writes `AUTOTEST trigger fired frame N: <ID> <name> house <house>` each time a trigger springs |
| `triggercoverage` | Writes each trigger type in the scenario with how many times it has sprung since the run started, then the types that never sprung. A long list runs over several lines, the later ones marked `(continued)` |
| `modsig [text]` | Writes the mods in force as a join request carries them: each folder name with a hash of its `mod.ini` and overlay files. With a signature as text, which is another machine's, also writes whether a host with these mods would take that machine in, and how the mods differ |
| `skipscore` | A won multiplayer or skirmish game passes over its score screen, as `SkipScoreScreen=yes` in a launch file does |
| `stall <ms>` | Holds the main thread for that long without pumping messages, so the hang watchdog reports it with the thread's call stack. The menu scripts have a `ui stall <ms>` step with the same effect |
| `quit` | Ends the process |

Every step writes an `AUTOTEST` line to the debug log. A flak shot writes `AUTOTEST scatter fire` (an arcing shell that is not invisible) or `AUTOTEST scatter moveto` (an invisible shell), with the range, distance, spread, angle draw and the aim point before and after the scatter, in leptons. The shell's burst writes `AUTOTEST flak impact`, at the cell where it detonates. A nuclear missile writes `AUTOTEST missile leaves` with the frame its silo launches it, `AUTOTEST nuke payload released` where its carrier bursts, `AUTOTEST nuke blast builds up` where the payload lands and `AUTOTEST nuke detonates` with the frame and coordinates of the blast. `ScreenCapture` and `record` save pictures to
the `Screenshots` folder of the user data directory, numbered in order, so a recording can be joined
into a video with `ffmpeg -framerate 30 -i SCRN%04d.png out.mp4`.

```text
30 command CenterBase
40 select AMCV
45 command DeployObject
310 produce GAPOWR
1500 place GAPOWR
2000 dump
2000 command ScreenCapture
2100 quit
```

### Waiting for conditions

A wait step pauses the script at its line until its condition holds or its timeout passes. The timeout is the optional last argument in the syntax above, in game frames counted from the frame the wait starts. It defaults to 3000 frames, about the length of a win run.

When the condition holds, the log has `AUTOTEST wait done at frame N: <line>` and the script goes on. When the timeout passes first, the log has `AUTOTEST wait timed out: <line>`, and the script still goes on, so the log shows where a run stopped. A line with an unknown type, trigger, house or local variable, or with too few arguments, logs `AUTOTEST wait problem: <reason>: <line>` once when it starts. It then waits until its timeout, as any other wait does.

Each wait adds the frames it took to every later line, so a line keeps its spacing from the wait line. A line ten frames after a wait line runs ten frames after the wait ends:

```text
100 waittrigger Start_Briefing 600
110 move MTNK 100 100
```

If `Start_Briefing` springs at frame 130, the wait ends at 130 and the move runs at frame 140. A timed-out wait counts its full timeout in the same way.

## Campaign missions

A campaign mission is played by naming it with `run_autotest.ps1 -Campaign -Map ALL01UMD.MAP`. The mission needs a window of at least 800x600, so add `-Width 800 -Height 600`.
The mission opens on its loading screen, which prints the briefing; no briefing page follows it, as in Yuri's Revenge, so a script needs no `ui` lines to start. The briefing
movies are skipped in an unattended run. The older scripts still begin with `ui click more` and `ui click resume` lines; with no page to click, each one logs `no such visible element`
and does nothing.

`tests/campaign/win-<mission>.txt` holds one script for each of the fourteen missions. Each forces what the map's triggers wait for, with `killhouse`, `killtag`, `own` and
`setlocal`, and the mission is won when the log has `AUTOTEST game over frame N: won`. The log also has an `AUTOTEST   sprung` line for every trigger that runs, with the frame it ran in.

## Skirmish lobby

`tests/autotest/skirmish-lobby-restore.txt` checks that the skirmish lobby keeps its setup after a
game and after a cancel. A `-Settings` text writes a `[Skirmish]` setup into the settings file: four
players on `XMP03T4.MAP`, which has four start spots. The script opens the lobby and starts a game.
`killhouse Spawn2 0`, `Spawn3 0` and `Spawn4 0` destroy the computers, `skipscore` passes over the
score screen, and the player wins. The lobby opens again, is cancelled, and opens a third time. The
log has three `AUTOTEST lobby` blocks, one for each opening. Each block is an `AUTOTEST lobby map`
line followed by one `AUTOTEST lobby row` line per row, and the three blocks must match.

```text
$s = "[Skirmish]`r`nMap=XMP03T4.MAP`r`nUnits=8`r`nCredits=5000`r`nTech=5`r`nSpeed=3`r`nBases=0`r`nCrates=1`r`nFog=1`r`nBridges=0`r`nRedeploy=0`r`nShortGame=0`r`nEngineer=1`r`nSuperWeapons=0`r`nBuildOffAlly=1`r`nSlot1=0,-1,0,0,0,0`r`nSlot2=3,2,1,2,1,0`r`nSlot3=4,0,2,3,1,0`r`nSlot4=2,-1,3,4,2,0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/run_autotest.ps1 -Build agent19 -Script src-agent19/tests/autotest/skirmish-lobby-restore.txt -Menus -MenuStyle Modern -Width 800 -Height 600 -Seconds 200 -Settings $s -Filter "AUTOTEST lobby|game over|Exception"
```

`tests/autotest/skirmish-lobby-fallback.txt` opens the lobby once and cancels it. Its `AUTOTEST lobby`
block shows what the lobby fell back to. Run it with a `-Settings` text that names a map that is not on
hand, or four rows on a two-start map such as `XMP29U2.MAP`. The lobby then shows the first map on hand,
or two rows.

The unit test `skirmishsetup` (CTest name `skirmishsetup`) restores saved rows onto maps with fewer
start spots and checks the fallbacks for a missing country or start, a kind past the list, and clashing
colors and starts. It needs no game data.

## Saving and loading

`command QuickSave` writes `QUICKSAVE.SAV` in the `Saved Games` folder of the user directory, and `command QuickLoad` loads it. A save made while the scenario's input is
locked writes nothing and logs nothing. A trigger's `LOCK_INPUT` action locks the input until an `UNLOCK_INPUT` action runs, and ALL01UMD.MAP locks it from its start until
about frame 1500. The log has `SAVING GAME [<file> - <description>]`, then `... - Complete`, and `LOADING GAME [<file>]` for a load. A save's description is `Quick Save: `
or `Auto-Save N: ` followed by the mission's name from its `UIName=` in `MISSIONMD.INI`.

Timed saves are off in a run started with `-SPAWN`, which `run_autotest.ps1` always passes, unless the `SPAWN.INI` it uses sets `AutoSaveGame=` to a number of frames. The script
writes its own `SPAWN.INI` without that key, so a timed save needs `-SpawnIni <file>` with the key added to its `[Settings]` section. The saves are then written as
`AUTOSAVE1.SAV` through `AUTOSAVE5.SAV`. A launch file's value replaces the player's `AutoSaveInterval` ([spawn-ini](/formats/spawn-ini/#automatic-saves)).

## Choosing the player's country

`run_autotest.ps1 -Country <index>` plays the player as that country, numbered by its place in the rules' `[Countries]` list. The stock list is `0` Americans (the default), `1` Alliance (Korea, which builds the Black Eagle), `8` Russians and `9` YuriCountry. The index is written as `Side=` in the generated `SPAWN.INI`, the same key a launch file uses for the player's country. The player's units and the start are the country's own: a Soviet player deploys `SMCV`, not `AMCV`. `-AICountries` still sets the computer players' countries.

## Comparing builds

`hash` covers what decides play: each structure, vehicle, aircraft and soldier's position, facing,
strength, mission, limbo state and owner, each house's credits, and the scenario's random-number
state. Logging it does not change the game. Run the same script, map and seed in two builds; the
first frame whose hash differs is where their gameplay diverged. A script that starts with
`1 hash 60` logs once a second at 60 game frames a second.

## Two-copy sync test

`scripts/run_netsync.ps1`, in the workspace that hosts this repository, runs two copies of one build as a two-player match over 127.0.0.1. It then reports whether the copies stayed in step. Both copies run hidden and muted, as in `run_autotest.ps1`.

```text
powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/run_netsync.ps1 -Build agent17 -Seconds 400
```

Each copy starts with `-SYNCLOG`, which writes one line to its debug log for every game frame:

```text
SYNC frame 400 crc 6134918a
```

The value is the frame check value the network already compares. A copy writes it into the frame info it sends, and the other copy checks it. While the copies agree, their logs hold the same value on every frame. The check value covers the position and facing of each infantry, unit and building, the position and type of each map and logic object, and the scenario's random-number state. It does not cover credits or other house state; the `hash` command does. The same log also has one `HOUSE_SYNC` line per house each frame, with its credits, tiberium value and build state, so a desync can be traced to house state. Each frame also has one `SYNC_DRAWS frame N draws D digest H` line. `D` counts the scenario's random draws since the previous frame's line, and `H` hashes their values and call sites. Equal lines mean equal draws, so the first differing `SYNC_DRAWS` line gives the first frame whose draws differ. With `-SYNCLOG`, each copy also writes timing and order lines; [NETCODE.md](NETCODE.md) gives their format.

The runner reports:

- the ticks each copy played, with its first and last frame;
- whether either copy reported an out-of-sync game, and the report files it wrote;
- the first frame whose check values differ;
- when a desync was reported, the first differing lines of the two out-of-sync reports, from the houses section on.

The exit code is `0` when the copies agree on every common frame, `3` when they disagree or a desync was reported, and `4` when the copies share no frame.

| Parameter | Default | Effect |
| --- | --- | --- |
| `-Build` | `src` | The build to run, named as in `run_autotest.ps1` |
| `-Map` | `XMP29U2.MAP` | The skirmish map |
| `-Seconds` | `120` | How long both copies run before the runner stops them |
| `-AI` | `2` | Computer players, seated the same way on both copies |
| `-AICountries` | the game's choice | Countries for the computer players in seat order, such as `5,6` |
| `-ScriptA`, `-ScriptB` | none | An autotest script for one copy |
| `-Seed` | `1` | The match seed, the same on both copies |
| `-PortA`, `-PortB` | `24601`, `24602` | The loopback ports the copies listen on |
| `-KeepDir` | none | A folder that receives both debug logs and any out-of-sync reports |
| `-ForceDesyncB` | `0` | Corrupts side B's check value at that frame, as `-DESYNCTEST=` does, to test the detection |
| `-SpawnStyle` | `Runner` | `CnCNet` writes the launch files the way the CnCNet client does, as [CnCNet style and tunnels](#cncnet-style-and-tunnels) describes |
| `-Tunnel` | off | Routes both copies through a local tunnel, with a bridge for each copy and a relay |
| `-TunnelIdA`, `-TunnelIdB` | `1001`, `1002` | The copies' tunnel numbers. Each is nonzero, and the two differ in their low 16 bits |
| `-BridgePortA`, `-BridgePortB`, `-RelayPort` | `24651`, `24652`, `24650` | The loopback ports of the two bridges and of the relay |
| `-LatencyMs`, `-JitterMs` | `0`, `0` | Delay added to each datagram on the relay hop. Jitter adds a random amount from minus to plus its value, so datagrams can arrive out of order |
| `-LossPercent` | `0` | Percentage of relay hops dropped |
| `-RelaySeed` | `1` | Seed for the jitter and the loss |
| `-RelayBuffer` | `1024` | The largest envelope the relay accepts; see below |

`tests/netsync/` holds one script per side for each map the test has been run on (`xmp29u2-side-a.txt` and `xmp29u2-side-b.txt` for `XMP29U2.MAP`, and the `xmp01t4` pair for `XMP01T4.MAP`). Pass them with `-ScriptA` and `-ScriptB`. The `XMP01T4.MAP` pair stayed in sync for 17,080 ticks in its latest run. Before the AI capture and disguise picks drew from the scenario's random stream, that pair desynced at frame 12061.

The `XMP29U2.MAP` pair desynced at frame 14726 in two of five runs before the mission-control fix below. The cause was a read outside the mission-control table. `MissionClass::Current_Mission_Control` indexed `MissionControl` with `MISSION_NONE` (-1), so an object with no mission took its `IsNoThreat` flag from memory before the table. That memory can differ between the two copies, so a target scan could accept an object on one copy and reject it on the other. The lookup now returns a record for `MISSION_NONE` with the values gamemd reads for it. gamemd indexes its table with -1 as well, with no bounds check: `TechnoClass::GetMissionControl` (0x005B3A00) returns `0xA8E3A8 + mission * 32`. The 32 bytes below the table, from 0xA8E388, belong to other globals. `NoThreat`, `Zombie`, `Recruitable` and `Paralyzed` sit at 0xA8E38C to 0xA8E38F, which no instruction addresses, and the section is zero there at load, so they are off. `Retaliate` and `Scatter` read the vtable pointer of a global object at 0xA8E390, so they are on. `Rate` and `AARate` read that object's capacity and count as denormal doubles, so the frame delay is zero. `MissionRules::NoMission*` holds these values, and `manual/changes/mission-none-threat-record.md` describes the effect. After the fix, five `XMP29U2.MAP` runs stayed in sync for 17,267 to 17,868 ticks each.

An earlier note put the first differing random draw at frame 14636. That came from comparing the draws by table cursor, which is offset between the copies. Compared in draw order, the draws agree through frame 14725. The first difference is at frame 14726, after a building's guard scan found a target on one copy only.

Each copy has its own user folder, `build\<Build>-netsync-a-user` or `-b-user`, with its own `SPAWN.INI`. The copy named `SyncA` is the host, and `SyncB` joins it. The runner seats two people, then the computer players, in the same order on both copies.

- `[Settings] BindAddress=127.0.0.1` makes each socket listen on loopback only, so nothing listens on the network.
- The runner writes `0 log NETSYNC_SIDE_A` (or `_B`) as the first line of each copy's script, and finds each copy's debug log by that marker.
- Both copies write to the build's `Debug` folder. Each run deletes the `DEBUG_*.LOG` and `SYNC_*.LOG` files there first, so use `-KeepDir` to keep a run's logs.
- Runs of one build take turns, as with `run_autotest.ps1`, and the runner holds two of the four game slots.

### Orders in a network game

In a network game (`Session.Type` is `GAME_IPX` or `GAME_INTERNET`), a step that gives an order queues it as an event, as a player's click does, so the other copy applies it too. Outside a network game, `move`, `attack`, `strike`, `strikeobj`, `rally` and `sell` change the object directly.

| Steps | In a network game |
| --- | --- |
| `produce`, `place`, `primary`, `stop`, `order`, `command DeployObject`, `clickcell`, `fire` | Already queue events, as the sidebar and the map clicks do |
| `move`, `attack`, `strike`, `strikeobj`, `rally`, `sell` | Queue an event: a mission for each object, an archive target or a sale |
| `planning 1`, `planning 0` | `planning 1` sets this copy's mode only. Orders given while the mode is on are recorded as planned orders, and `planning 0` queues the commit that starts them |
| `beacon place x y`, `beacontext <text>` | Queue the beacon event: a placement on the cell, or the text of the selected beacon. `beacon select x y` and `beacon delete` change this copy's selection, and `beacon delete` queues a removal only for a beacon the player owns |

A step that changes an object or a scenario value without an event changes this copy only. These include `own`, `spawn`, `grantunit`, `neutral`, `kill`, `killhouse`, `killtag`, `hurt`, `grant`, `grantai`, `transfer`, `cmove`, `setlocal`, `setglobal` and `trigaction`. A network test must not use them, because the other copy keeps its old state: with `1000 own E1 60 60` on one side only, the runner reports a desync at frame 1000. Steps that only read state or change this copy's view (`select`, `view`, `follow`, `record`, `log`, `dump`, `where`, `census`, `supers`, `canbuild`, `planstate`) do not affect the other copy.

The out-of-sync comparison is positional: it compares line N of one report with line N of the other. One object that appears only on one side shifts every line after it, so read the first difference, not the whole list.

`-ForceDesyncB` is the check that the runner detects a desync. With `-ForceDesyncB 400`, the first mismatching tick is 400 and the runner exits with `3`.

### CnCNet style and tunnels

`-SpawnStyle CnCNet` writes the launch files the way the CnCNet client writes them, for the keys whose values are known. `UIGameMode`, `BroadcastedGameOptionValues` and `GamePackageVersion` are placeholders, `GameID` is a placeholder, and `MapSHA1` is empty. The client copies the map to `spawnmap.ini`; here `Scenario` names the map in the game data instead. Each copy gets `-SPAWN` as the client passes it, plus the harness switches `-DATADIR`, `-USERDIR`, `-WIN`, `-MULTIINSTANCE` and `-AUTOTEST`. The sync log comes from `SyncLog=yes` in `RA2MD.INI`, not from `-SYNCLOG`. A direct copy gets `BindAddress=127.0.0.1`, which the client does not write, so the copy listens on loopback only.

`-Tunnel` starts `scripts/cncnet_tunnel_relay.py` with `python -I`. The script runs one bridge per copy and one relay on 127.0.0.1. A bridge wraps each game datagram in an 8-byte envelope and sends it to the relay. The relay forwards the envelope to the bridge of the peer that the envelope names. The relay stops after `-Seconds` plus three seconds and prints a summary line with its counters, which the runner repeats.

Two results from testing set the limits of a tunnel run:

- In tunnel mode the game's socket binds an ephemeral port on every address. The engine ignores `BindAddress` there, so the runner's listing shows `0.0.0.0`.
- A game datagram is at most 1016 bytes: the server's 1024-byte buffer less the bridge's 8-byte envelope. The game does not send a larger datagram in tunnel mode and logs each refused size once. Global-channel packets carry only the bytes their command reads, so the largest one is 1016 bytes on the wire. A relay with a 1024-byte limit therefore passes every datagram, and the tunnel runs use `-RelayBuffer 1024`. Before this limit, every global-channel message was 1076 bytes and a 1024-byte relay dropped them all. Whether the real server drops or truncates an oversized datagram is not confirmed.

Check the relay's routing before a match:

```text
python -I scripts/cncnet_tunnel_relay.py --self-test
```
