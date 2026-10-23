# Performance timing

`[Options] PerfLog=yes` in `RA2MD.INI`, or the `perflog 1` autotest step, makes the game write
timing and count lines to the debug log once a second. They show where a game frame, a draw and a
present spend their time. The code is in `code/perfstat.h` and `code/perfstat.cpp`.

## Recording

Recording is off by default. While it is off, each timed scope costs one flag test. Turning it on
clears the counts and starts a new second. Turning it off stops the lines; the timers it had
started finish without a sample.

The three kinds of sample:

| Kind | One sample is | Phases |
|---|---|---|
| Frame | One game frame, from `Main_Loop` after `Sync_Delay` | `frame.*`: logic, its parts, and the wait between frames |
| Draw | One call to `GScreenClass::Render` | `draw.*`: terrain, objects, effects, interface |
| Present | One `Present` call that reaches the presenter | `present.*`: texture upload and the bgfx frame |

A phase nested in another phase also adds to the outer one. `frame.objects` contains
`frame.pathfind` and `frame.threat`, and `draw.objects` contains `draw.voxels` and `draw.particles`.
`draw.total` includes the present that a draw starts, so draw and present time overlap.

## Perf lines

Each second the log gets four lines, followed by the count line:

```text
Perf: window 1000 ms, 60 game frames, 233 draws, 184 presents
Perf frame ms avg/p95/max: logic 1.36/2.06/2.61, ...
Perf draw ms avg/p95/max: total 4.74/7.34/38.89, ...
Perf present ms avg/p95/max: total 2.34/3.23/17.14, ...
Perf count: objects 32925, voxels 4120, ...
```

Each phase reads `average/p95/worst` in milliseconds over the samples taken in that second. The p95
is the nearest-rank 95th percentile. Counts are totals for the second.

| Count | Meaning |
|---|---|
| `objects` | Objects that `Tactical::Draw_Objects` visits, not counting terrain objects |
| `voxels` | Voxel draws of a body or turret, not counting shadows |
| `voxel hit`, `voxel miss` | Cache-eligible voxel draws found in or missing from the cache, when the draw is not between game frames |
| `voxel smooth hit`, `voxel smooth miss` | The same, for draws between game frames, where the facing is blended toward the previous frame's |
| `voxel uncached` | Voxel draws that do not use the cache, such as turrets |
| `particles` | Particle draws |
| `paths` | Path searches (`AStarClass::Find_Path`) |
| `threats` | Target scans (`TechnoClass::Greatest_Threat`), counted at the base class |
| `object ai` | Object AI calls |
| `upload bytes`, `uploads` | Bytes and calls of texture uploads in the bgfx backend |

## Benchmark

`tests/autotest/bench-large-battle.txt` runs a fixed battle for 7200 game frames on XMP08T2 and then
quits. It places 336 objects on frame 10: the player and three computer houses each get 40
vehicles, 40 infantry and two aircraft, and eight destroyers sit on the lake. The houses are named
with `houseunit` by their position in the house list (`#1` to `#3`). From frame 600 on, the computer
houses hunt, the player moves its forces across the map, and focus-fire orders go out.
`bench-large-battle-zoomed-out.txt` is the same battle with the view set to `zoom 0.5`.

```text
powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/perf_bench.ps1 -Build agent14 -Runs 3
```

The script runs `scripts/run_autotest.ps1` in the workspace that holds the worktree, at 1920x1080
with `RenderFrameRate=240`, three computer players and game speed 1. It copies each run's debug log
to `scratch-agent\perf` and prints a summary table of frames, draws and presents per second, and the
mean, p95 and worst time for each phase and the mean count per second. The first five seconds of
each run are left out. `-Logs <file>...` summarises logs that already exist.

The summary's p95 is the 95th percentile of the per-second p95 values, and its max is the largest
per-second worst. These are not percentiles of every sample in the run.

## Limits

- Each timer reads a clock twice, so the timed phases run slower than they do with recording off.
  Compare runs with the same setting.
- The clock is `std::chrono::steady_clock`. On Windows it is the performance counter.
- Only the phases listed above are timed. Work outside them is not in any phase. To find it, subtract
  the child phases from their parent; `draw.total` also covers the screen copy before the present.
