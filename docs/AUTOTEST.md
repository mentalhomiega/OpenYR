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
| `select <TypeID>` | Selects every object of that type the player controls: the player's house, and any house the map marks `PlayerControl=yes` |
| `produce <TypeID>` | Starts building that type, as a click on its cameo does |
| `place <TypeID>` | Places the finished structure on the first legal cell found around the player's construction yard; a finished vehicle, aircraft or infantryman is released from its factory as the sidebar does |
| `grantunit <TypeID> x y` | Like `spawn`, but the object belongs to the player, so `move` and `strike` order it |
| `move <TypeID> <x> <y>` | Orders the player-controlled objects of that type (see `select`) to the cell |
| `attack <TypeID>` | Orders the player-controlled objects of that type (see `select`) to attack the nearest structure of another house that has a construction yard |
| `view <x> <y>` | Centres the view on the cell |
| `follow <TypeID>` | Keeps the view centred on one of the player's objects of that type, until the next `view` |
| `record <frames>` | Saves a screenshot every that many frames; `record 0` stops |
| `dump` | Writes the player's credits, structures and units, with their missions and movement, to the debug log |
| `enemies` | Writes the other houses' structures and their unit and infantry counts to the debug log |
| `iq` | Writes each house's base IQ (the map's `IQ=`, 0 for a skirmish computer), its effective IQ, and whether each [IQ] gate is open for it |
| `owners <TypeID>` | Writes the type's owner bits and each house's country bit to the debug log |
| `anims` | Writes the first entries of the animation list to the debug log |
| `truecolour <NAME.SHP>` | Writes whether a PNG replaces that shape (see [TRUECOLOUR.md](TRUECOLOUR.md)), whether the shape is an SHP (`shp`) or was made from a sheet that has none (`png-only`), with the shape's frame count and size, the sheet's size, the frames it covers, whether it has a house-colour mask, whether the shadows come from the PNG or the SHP, and the centre pixel of its first frame |
| `truecolourdir <path>` | Adds a directory to the places the game looks for files, so a test can supply PNG sprites without copying them into the game directory; a PNG not found before is looked for again |
| `exportshape <NAME.SHP> <out.png>` | Writes every frame of that shape to a PNG sheet in the layout [TRUECOLOUR.md](TRUECOLOUR.md) reads, coloured with the current theater's unit palette, with index 0 transparent; when the shape has house-colour pixels, also writes `<out>_hc.png`, the matching house-colour mask. The output path is the rest of the line |
| `loadshot <name>` | Saves what the window shows each time a loading screen is complete, as `<name>-<count>.tga` in the `Screenshots` folder. It takes effect when the script is read, before the first game frame, whatever frame the line names |
| `triggers [all]` | Writes the trigger types with their owner, the events they wait for and whether a live trigger of each is enabled (only the enabled ones, unless `all`), then every tag with the objects and cells it rides on, and the local and global variables that are set |
| `typecounts` | Writes how many structure, vehicle, soldier and aircraft types there are, and any whose ID is not a plain name, which is how a list read as one name shows up |
| `where <TypeID>` | Writes each object of that type, whoever owns it, with its cell, mission, destination, target and strength; a soldier also gets a line with its action and whether it is dug in |
| `quantity <TypeID>` | Writes how many of that type each house is counted as having, owned and active, which are the two counts the map events read. A structure uses the building counts, a unit, infantry or aircraft type its own |
| `where <TypeID>` | Writes each object of that type, whoever owns it, with its cell, mission, destination, target and strength; a soldier also gets a line with its action and whether it is dug in; a teleporting object gets its warp phase (0 idle, 1 warping out, 2 holding), the frame the phase ends, whether it is warping, whether a chrono warp holds it, and whether it is selected |
| `quantity <StructureID>` | Writes how many of that structure each house is counted as having, owned and active, which are the two counts the map events read |
| `killhouse <House> [kind]` | Destroys the buildings (kind 1), the vehicles, soldiers and aircraft (kind 2) or everything (kind 0, the default) the house owns, with one of the player's objects as the attacker. An underscore in the name stands for a space |
| `killtag <Tag>` | Destroys every object that carries a tag of that ID or name, with one of the player's objects as the attacker |
| `transfer <TypeID>` | Gives the first live object of that type that a computer house owns to the player through `TechnoClass::Set_Owning_House`, the path a mind control capture takes |
| `kill <TypeID> [1]` | Destroys every computer object of the type with no attacker, the C4 warhead forced; with a `1`, every player object of the type instead |
| `setlocal <index> <value>`, `setglobal <index> <value>` | Sets a scenario variable when the value is 1 and clears it when it is 0, as a trigger action does |
| `hash [frames]` | Writes a hash of the game state to the debug log, and again every that many frames when given; `hash 0` logs once and stops the repeats |
| `sell <x> <y>` | Starts selling the player's structure on that cell, as the sell cursor's click does |
| `infiltrate barracks\|warfactory` | Marks the player's house as having spied on that building, so its new trainable infantry or units start as veterans |
| `occupy <TypeID> x y` | Puts the player's first infantry of that type inside the structure on that cell, without walking there |
| `neutral <TypeID> x y` | Puts an object of the type on that cell, owned by the house of the Civilian side, as `own` does for the player |
| `rank <TypeID> <0\|1\|2>` | Makes every object of that type, whoever owns it, rookie, veteran or elite |
| `veterancy <TypeID>` | Writes the rules' `VeteranRatio` and `VeteranCap`, the player's score, then each object of that type with its rank, experience and cost |
| `crate <Powerup> x y` | Puts a crate that holds that powerup, named as in the crate list (`money`, `unit`, `heal`, `cloak`, `explosion`, `napalm`, `squad`, `darkness`, `reveal`, `armor`, `speed`, `firepower`, `icbm`, `invuln`, `veteran`, `ion`, `gas`, `tiberium` or `pod`), on the nearest free cell to the cell |
| `unload <x> <y>` | Starts the unload mission of the player's structure on that cell. A vehicle on a cell with no structure unloads its passengers instead |
| `rally <TypeID> x y` | Sets the rally point of the player's structures of that type to the cell, without the nearby-cell search an Alt-click makes |
| `census cells` | Writes the eight tiberium cells nearest the player's first building, nearest first, with their squared distance in cells |
| `census ore` | Writes, for each tiberium type, the cells holding it with their total density and credit value, how many of them can still grow or spread, and the lengths of its growth and spread queues. `census` writes the houses instead |
| `rallyclick <TypeID> x y` | Gives the player's structures of that type the rally click of an Alt-click on the ground, with the nearby-cell search |
| `cell <x> <y>` | Writes the cell's map state, then its building and its occupier |
| `log <text>` | Writes the line to the debug log |
| `rule <Name> <value>` | Changes a rule for the rest of the run: `CanDetonateTimeBomb` (0 or 1) and `BallisticScatter` (leptons) |
| `strike <TypeID> x y` | The player's objects of that type attack the object on that cell |
| `treehit <Warhead> x y` | Hits the terrain object on that cell with 100 points of damage from the warhead, with no firer, and writes its strength before and after |
| `trees` | Writes every terrain object on the map with its cell, strength, armor and whether it is immune |
| `quit` | Ends the process |

Every step writes an `AUTOTEST` line to the debug log. A flak shot writes `AUTOTEST scatter fire` (an arcing shell that is not invisible) or `AUTOTEST scatter moveto` (an invisible shell), with the range, distance, spread, angle draw and the aim point before and after the scatter, in leptons. The shell's burst writes `AUTOTEST flak impact`, at the cell where it detonates. `ScreenCapture` and `record` save pictures to
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

## Campaign missions

A campaign mission is played by naming it with `run_autotest.ps1 -Campaign -Map ALL01UMD.MAP`. The mission needs a window of at least 800x600, so add `-Width 800 -Height 600`. The
script starts with `ui` lines, which are played in file order before any game frame: five `ui click more` with a `ui wait 300` after each, then `ui click resume`, click through the briefing page.
The briefing movies are skipped in an unattended run.

`tests/campaign/win-<mission>.txt` holds one script for each of the fourteen missions. Each forces what the map's triggers wait for, with `killhouse`, `killtag`, `own` and
`setlocal`, and the mission is won when the log has `AUTOTEST game over frame N: won`. The log also has an `AUTOTEST   sprung` line for every trigger that runs, with the frame it ran in.

## Comparing builds

`hash` covers what decides play: each structure, vehicle, aircraft and soldier's position, facing,
strength, mission, limbo state and owner, each house's credits, and the scenario's random-number
state. Logging it does not change the game. Run the same script, map and seed in two builds; the
first frame whose hash differs is where their gameplay diverged. A script that starts with
`1 hash 60` logs once a second at 60 game frames a second.
