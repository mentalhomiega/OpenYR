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
| `select <TypeID>` | Selects every object of that type the player owns |
| `produce <TypeID>` | Starts building that type, as a click on its cameo does |
| `place <TypeID>` | Places the finished structure on the first legal cell found around the player's construction yard |
| `move <TypeID> <x> <y>` | Orders the player's objects of that type to the cell |
| `attack <TypeID>` | Orders the player's objects of that type to attack the nearest structure of another house that has a construction yard |
| `view <x> <y>` | Centres the view on the cell |
| `follow <TypeID>` | Keeps the view centred on one of the player's objects of that type, until the next `view` |
| `record <frames>` | Saves a screenshot every that many frames; `record 0` stops |
| `dump` | Writes the player's credits, structures and units, with their missions and movement, to the debug log |
| `enemies` | Writes the other houses' structures and their unit and infantry counts to the debug log |
| `owners <TypeID>` | Writes the type's owner bits and each house's country bit to the debug log |
| `anims` | Writes the first entries of the animation list to the debug log |
| `log <text>` | Writes the line to the debug log |
| `quit` | Ends the process |

Every step writes an `AUTOTEST` line to the debug log. `ScreenCapture` and `record` save pictures to
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
