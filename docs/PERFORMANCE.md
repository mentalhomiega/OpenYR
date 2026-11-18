# Performance timing

`[Options] PerfLog=yes` in `RA2MD.INI`, or the `perflog 1` autotest step, makes the game write
timing and count lines to the debug log once a second. They show where a game frame, a draw and a
present spend their time. The [performance overlay](#performance-overlay) shows the same timings on
screen. The code is in `code/perfstat.h` and `code/perfstat.cpp`.

## Recording

Recording is off by default. It runs while the Perf lines or the performance overlay are on. While
it is off, each timed scope costs one flag test. Turning recording on clears the counts and the
overlay's history and starts a new second; turning the Perf lines on while the overlay already
records starts a new second of lines and keeps the history. Turning recording off stops the lines;
the timers it had started finish without a sample. The overlay alone writes no Perf lines.

The three kinds of sample:

| Kind | One sample is | Phases |
|---|---|---|
| Frame | One game frame, from `Main_Loop` after `Sync_Delay` | `frame.*`: logic, its parts, and the wait between frames |
| Draw | One call to `GScreenClass::Render` | `draw.*`: terrain, objects, effects, interface |
| Present | One `Present` call that reaches the presenter | `present.*`: texture upload and the bgfx frame |

A phase nested in another phase also adds to the outer one. `frame.objects` contains
`frame.pathfind` and `frame.threat`, and `draw.objects` contains `draw.voxels` and `draw.particles`.
The phase table in `code/perfstat.cpp` names each phase's parent; the overlay builds its breakdown
from it. `frame.objects` covers the object AI loop alone. Before 2026-11-12 its scope ran to the end
of `LogicClass::AI`, so older `frame.objects` numbers also hold `map`, `tactical`, `factories` and
`houses`.
`frame.threat` contains `frame.ring` (the cell ring walk of an area or range scan) and `frame.eval`
(the `Evaluate_Object` calls that the ring walk makes for the occupant of a cell). Those calls are the only
ones in `frame.eval`; the full-map scan's calls are in `frame.threat` alone. The ring walk's own cost,
without its occupant evaluations, is `frame.ring` minus `frame.eval`. That remainder holds the per-cell tests
and `Evaluate_Just_Cell`, which are not timed separately, because a clock read per cell would cost more than
the cell.
`draw.total` includes the present that a draw starts, so draw and present time overlap.

## Perf lines

Each second the log gets a window line, the frame, draw, present and gap lines, and the count line:

```text
Perf: window 1000 ms, 60 game frames, 233 draws, 184 presents
Perf frame ms avg/p95/max: logic 1.36/2.06/2.61, ...
Perf draw ms avg/p95/max: total 4.74/7.34/38.89, ...
Perf present ms avg/p95/max: total 2.34/3.23/17.14, ...
Perf gap ms avg/p95/max: present 16.68/17.05/31.20
Perf count: objects 32925, voxels 4120, ..., draws skipped 3
Pacing: 60 ticks in 1000.0 ms, interval ms min/mean/max/sd 16.640/16.667/16.702/0.012, cpu 85.2 ms
```

Each phase reads `average/p95/worst` in milliseconds over the samples taken in that second. The p95
is the nearest-rank 95th percentile. Counts are totals for the second.

The `Pacing` line times the game frames themselves. `interval` is the time between the ends of two
consecutive frame waits: its minimum, mean, maximum and standard deviation over the second, which show
how evenly the game is paced. A campaign or skirmish at game speed 1 should read a mean of 22.222 ms (45 frames a second) and at game speed 2 a mean of 33.333 ms (30 frames a second); with `ClassicGameSpeeds=yes` they read 16.000 ms and 32.000 ms, and 32.000 ms in any campaign. A slow frame is not made up by shorter ones, so a mean above the target is expected after a spike and a mean below it never is. `cpu` is the
processor time (kernel and user, all threads) that the process used in the window, so the waits are
idle when it stays well below the window length.

`present.gputime` is the GPU time of the newest frame the GPU finished, from `bgfx::getStats`. bgfx
measures it a frame or more behind the present that records it, so read it as the GPU cost per present,
not as part of `present.total`. A present whose renderer gives no GPU timer adds zero.

The gap line has one sample per present that reaches the presenter: the time from that present's start
to the start of the next one. Its worst value is the longest interval between two presents in the second.

## Performance overlay

The performance overlay shows the frame rates, two frame-time graphs and the time of each phase over
the top left of the map view. It is for testers: turn it on where the game runs badly and take a
screenshot for the report. The code is in `code/ui/dev/uiperfoverlay.cpp`.

- The `PerfOverlay` command cycles it through Basic, Detailed and off. Its default key is Ctrl+Shift+P
  in both control schemes, and the keyboard page rebinds it like any other command.
- `PerfOverlay=Off|Basic|Detailed` under `[Video]` sets the level at start. The Graphics page shows it as
  the Performance overlay switch, which picks Basic. The key changes the same setting, so the next save
  of the settings keeps the level the key left.

It is drawn with Dear ImGui at the window's own resolution, after the frame, so it shows with
`GpuRender` on and off, in both interfaces and with the render thread. Its text is about 13 pixels high
when the frame is shown at its own size and is enlarged with the frame, to twice that at 3840x2160
with the automatic interface scale. It shows only while a game runs,
not over the menu battle or an open menu. It stays inside the map view, left of the modern build panel
and above the command bar; where the Detailed level is taller than that space, its last lines are cut.

| Level | Shows |
|---|---|
| Basic | The game and drawn frame rates with their frame times, the two graphs, the build and the overlay's own cost |
| Detailed | Also the time of each phase, the counts, and the window, zoom, renderer, `GpuRender`, interface and render thread |

### What the numbers are

Every number covers the samples of the last 5 seconds, at most 2,048 of each kind.

| Number | How it is computed |
|---|---|
| Game fps | 1000 divided by the average time from the end of one game frame to the end of the next, the interval the `Pacing` line measures |
| Game now, avg, worst | The newest, average and largest of those intervals |
| 1% low | 1000 divided by the nearest-rank 99th percentile of the intervals: the rate the slowest 1% of frames ran at |
| logic avg, max | `frame.logic` per game frame |
| Drawn fps, now, avg, worst, 1% low | The same, from the time between the starts of two presents, as on the `Perf gap` line |
| draw avg, max | `draw.total` per draw; it includes the present the draw starts |

The graphs show the last 300 frames, the newest at the right, one column each, on a fixed scale of 0
to 50 ms with dashed lines labelled 16.7 ms (60 frames a second) and 33.3 ms (30 a second). A column
over 50 ms is cut at the top and marked with a triangle above it. In the game graph the grey column is
the interval and the white part of it is the logic. The drawn graph shows the time between presents.

The breakdown has one row per phase of the phase table, the game frame on the left and the draw and
present on the right. A row shows the average and worst time per sample of its kind and a bar of the
average, full width at 16.7 ms. An indented row is part of the row above it, so a row and its indented
rows do not add up. Rows marked `~` (`pathfind`, `threat`, `detach`) run inside several rows: the path
search mostly in `objects`, the target scans in `objects`, `teams` and `houses`, and `detach` in whatever
removed the object. `gputime` is not part of the present's `total`. A row whose average is under
0.005 ms and whose worst is under 0.05 ms is left out.

The counts are the totals of the last whole second, as on the `Perf count` line, and the number of
objects and of units and structures in the game.

A game frame interval includes any time no game frame ran, such as a held window capture or a
paused game, so the frame after it shows as one long interval.

### Cost

While the overlay is off, it costs the flag test of each timed scope and one test of the level each
interface pass. While it is on, recording is on, so the timed scopes read the clock as they do with
`PerfLog=yes`. Building the overlay took 0.07 to 0.19 ms at Basic and 0.17 to 0.37 ms at Detailed per
game frame, and drawing it 0.004 to 0.023 ms per present, at 3840x2160 and 800x600, with other work
on the machine. The drawing is part
of `present.total`. The overlay's last line shows both times. In two alternating pairs of the first
1,600 frames of the large battle at 3840x2160 with `GpuRender` and `RenderFrameRate=240`, Detailed
against off with the Perf lines on in both, the process used 582 and 629 ms of processor time a second
against 565 and 589, and the phase averages differed by less than the noise between the pairs.

The overlay reads the timings and counts and changes nothing the simulation reads.

The `perfoverlay <0|1|2|report>` autotest step sets the level or logs the overlay's numbers
([AUTOTEST.md](AUTOTEST.md)).

## Skipped draws

With `RenderFrameRate` above zero, the wait between two game frames redraws the view each
`1000 / RenderFrameRate` ms, timed on the nanosecond clock. A due redraw is skipped when less time is left in the wait than the last
redraw took (`mainloop.cpp`, `Sync_Delay`). `draws skipped` counts a skip at most once for each interval
between two redraws that happen, so a due redraw that is skipped again and again adds one. The counter stays at zero with `RenderFrameRate=0`, which has no such loop.

| Count | Meaning |
|---|---|
| `objects` | Objects that `Tactical::Draw_Objects` visits, not counting terrain objects |
| `voxels` | Voxel draws of a body or turret, not counting shadows |
| `voxel hit`, `voxel miss` | Cache-eligible voxel draws found in or missing from the cache, when the draw is not between game frames |
| `voxel smooth hit`, `voxel smooth miss` | The same, for draws between game frames, where the facing is blended toward the previous frame's |
| `voxel uncached` | Voxel draws that do not use the cache: aircraft bodies, a body with no cacheable pose (for example tilting), a turret part while its recoil is under way, and types with `DisableVoxelCache=yes` |
| `particles` | Particle draws |
| `paths` | Path searches (`AStarClass::Find_Path`) |
| `threats` | Target scans (`TechnoClass::Greatest_Threat`), counted at the base class |
| `object ai` | Object AI calls |
| `upload bytes`, `uploads` | Bytes sent to textures, and the texture updates that sent them. A texture update sends only the tiles that changed since the last one, so a still picture adds nothing |
| `scans ring`, `scans air`, `scans full` | Target scans by path: area and range scans that walk the cell ring, scans that walk the aircraft and air-layer lists, and full-map scans. One scan can count in both `scans ring` and `scans air` |
| `scan targeting`, `scan mission`, `scan team`, `scan house` | `Greatest_Threat` calls from `Target_Something_Nearby`, from foot area scans (attack move, patrol, guard area), from team AI, and from house AI (quarry). The remaining scans are in `threats` but in none of these four |
| `ring cells` | Ring cells on the radar map that the cell walk visits, counted per area or range scan before any evaluation |
| `cell candidates` | `Evaluate_Cell` calls that found an occupant and passed it to `Evaluate_Object` |
| `object evals` | `Evaluate_Object` calls, from the ring walk and from full-map scans |
| `area modify` | `Area_Modify` calls, including the calls that return 1 at once for a weapon that is not suppressive |
| `weapon selects` | `What_Weapon_Should_I_Use` calls |
| `draws skipped` | Redraws that were due in the wait between game frames but had too little time left. See [Skipped draws](#skipped-draws) |
| `presents busy` | Presents put off because the render thread had not finished the last frame. It stays at zero without `RenderThread`. See [RENDER_THREAD.md](RENDER_THREAD.md) |
| `gpu voxel parts`, `gpu voxel direct`, `gpu voxel faces` | With `GpuVoxels=yes`, the voxel parts of the passes the GPU drew, those of them a blit shows, and the cube faces their draws submitted, counted only at a voxel scale above 1 and `GpuVoxelLayerDepth=Shared`. See [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md) |
| `logic over 20 ms`, `logic over 33 ms`, `logic over 50 ms` | Game frames whose `frame.logic` took longer than 20, 33 or 50 ms. A frame over 50 ms counts in all three |

The `ring cells` counter is taken only when the cell walk runs. Each radar-map ring cell the walk visits adds one
counter increment while recording.

## Benchmark

`tests/autotest/bench-large-battle.txt` runs a fixed battle for 7200 game frames on XMP08T2 and then
quits. It places 336 objects on frame 10: the player and three computer houses each get 40
vehicles, 40 infantry and two aircraft, and eight destroyers sit on the lake. The houses are named
with `houseunit` by their position in the house list (`#1` to `#3`). From frame 600 on, the computer
houses hunt, the player moves its forces across the map, and focus-fire orders go out.
`bench-large-battle-zoomed-out.txt` is the same battle with the view set to `zoom 0.5`; on XMP08T2 that is zoom 1.00 at 3840x2160, because the map is small.
`bench-large-battle-xmp04-zoomed-out.txt` is a battle of the same size on XMP04T8, the largest stock map, with the map revealed, for 3840x2160 at the furthest zoom (0.50, a 7546x4245 view); run it with `-Map XMP04T8.MAP -Width 3840 -Height 2160` and three computer players.
`bench-large-battle-revealed.txt` and `bench-large-battle-revealed-zoomed-out.txt` reveal the map first, and `bench-large-battle-scrolling.txt` moves the view around the middle of the map every four game frames; these three stop at frame 3600.

```text
powershell.exe -NoProfile -ExecutionPolicy Bypass -File scripts/perf_bench.ps1 -Build agent14 -Runs 3
```

The script runs `scripts/run_autotest.ps1` in the workspace that holds the worktree, at 1920x1080
with `RenderFrameRate=240` (`-Width`, `-Height` and `-RenderFrameRate` change them), three computer players and game speed 1. It copies each run's debug log
to `scratch-agent\perf` and prints a summary table of frames, draws and presents per second, and the
mean, p95 and worst time for each phase and the mean count per second. The first five seconds of
each run are left out. `-Logs <file>...` summarises logs that already exist.

The summary's p95 is the 95th percentile of the per-second p95 values, and its max is the largest
per-second worst. These are not percentiles of every sample in the run.

## Path search benchmarks

Two benchmarks load the path search, which the small-map large battle barely does (about 31 searches a second).

- `tests/autotest/bench-bigmap-crossing.txt` puts two armies of 90 vehicles and 90 infantry about 200 cells apart on XMP22S8 and sends them across the map for 7200 frames. Run it with `scripts/perf_bench.ps1 -Build agent27 -Script src-agent27/tests/autotest/bench-bigmap-crossing.txt -Map XMP22S8.MAP -AIPlayers 1`.
- `tests/autotest/bench-mission-all02.txt` plays Yuri's Revenge Allied 2 with a 240-unit army. Add `-Map ALL02UMD.MAP -Campaign`.

On the big-map crossing, `frame.pathfind` is 68% of `frame.logic` (about 2.3 of 3.5 ms). A search spends about 22% in the hierarchical zone search and 78% in the cell search; `Can_Enter_Cell` is about 35% of the whole search. Retries and searches without the zone corridor are under 0.2% of searches there; in ALL02 most cell searches fail and retry. The worst frames (about 110 to 140 ms in both benchmarks) are one frame with 255 to 270 searches, started when a group is ordered across the map. No single search takes more than 3.4 ms.

## Path search speed-ups

Two changes leave every path as it was.

- **Corridor cache.** A first-try zone search for a unit that does not avoid threats depends only on its start and goal zones on the three levels, its movement zone and the zone graph. `AStarClass::Find_Path_Hierarchical` keeps the result of such a search in a 1,024-slot table and, on a repeat, writes the same path lists, counts and corridor stamps without searching. Retries (banned edges), failures and threat-avoiding units are never cached. Every function that changes the zone graph calls `Search.Invalidate_Hierarchical_Cache`; so do `AStarClass::Reset` and `Update_Map_Dimensions`. On the big-map crossing 88% of lookups hit.
- **Prefetch.** When the cell search expands a node, it starts the cache misses for the eight neighbours' cells, zone entries and visit stamps together before it reads them.

Two environment variables are for debugging. `OPENYR_ASTAR_CACHE=0` turns the cache off. `OPENYR_ASTAR_CHECK=1` runs the real zone search beside every hit and logs `AStar cache check: ... differences` every 5,000 comparisons and a `DIFFERENCE` line for any mismatch. The big-map crossing compared 45,000 hits and ALL02 40,000 hits with no difference.

Means of three runs, with other load on the machine (ms):

| Benchmark | `frame.pathfind` before | after | `frame.logic` before | after | p95 logic after | worst logic after |
|---|---:|---:|---:|---:|---:|---:|
| Big-map crossing | 2.31 | 1.84 | 3.51 | 3.05 | 12.0 | 116 |
| ALL02 mission | 0.57 | 0.42 | 2.24 | 1.67 | 6.3 | 134 |
| Large battle | 0.008 | 0.006 | 0.99 | 0.97 | 1.9 | 13 |

The big-map "before" is the cache turned off, before the prefetch; the ALL02 "before" is a separate single run of the original code.

## Parallel path searches

`ParallelPaths=yes` searches the paths of walking and driving units in batches on several threads. It is off by default. With it on, paths and the order of some first steps differ from the original game's, so it is a rule of the match: every player plays the same value.

| Game | Where the value comes from |
|---|---|
| Campaign or skirmish started from the menus | `ParallelPaths` in `[Options]` of `RA2MD.INI` |
| Any game started from a launch file | `ParallelPaths` in `[Settings]` of the launch file |
| Multiplayer started from the menus | Always off for now |
| Saved game | The value the game was started with |

`PathThreads` in `[Options]` of `RA2MD.INI` is the number of threads on this machine, 1 to 32. The default 0 uses one less than the number of processor threads, at most 8. The thread count does not change any path, so players in one match can use different counts.

Both `RA2MD.INI` keys have a row on the Performance settings page: Parallel path searches and Path search threads.

A frame makes two batches:

- **Before the units move.** Each walking or driving unit that will ask for a path in its turn gets that path searched from the map as it stands at the start of the frame. The unit takes the path when it asks with the same start, goal and avoidance.
- **After the units move.** A unit that asks for a path with none made for it stops its movement step and goes on a list. The list is searched after the last unit's turn, and each listed unit then repeats its movement step in list order. It starts moving in the same frame; only its place in the frame's order changes.

A player's order reaches its units between frames, so its paths are in the first batch of the next frame, the frame in which the original game searches them. A path a unit asks for during the second batch's repeat steps, and any path of another locomotor, is searched at once on the game thread.

Late-game benchmark, 8 threads, one run each (ms; the two games play out differently, so the figures compare whole runs):

| | `frame.pathfind` mean | worst one-second mean | `frame.logic` mean |
|---|---:|---:|---:|
| Off | 3.00 | 88.7 | 6.81 |
| On | 1.30 | 26.3 | 4.14 |

With it on, 86% of the path requests took a path from a batch. The largest second batch held 370 searches and took 16 ms on 8 threads and 104 ms on 1.

Four environment variables are for development. `OPENYR_PATH_AHEAD=<threads>` overrides the rule and the thread count; 0 turns the batches off. `OPENYR_PATH_THREADS=<threads>` sets the thread count alone; `scripts/run_netsync.ps1 -ParallelPaths -PathThreadsA 1 -PathThreadsB 8` uses it to pair two counts in one match. `OPENYR_PATH_AHEAD_CHECK=1` repeats every batch on one thread with a separate pathfinder and logs `PATHAHEAD CHECK DIFFERENCE` for any path that differs. `OPENYR_PATH_AHEAD_STATS=1` logs a `PATHAHEAD frame` line of counts every 600 frames. On the late-game benchmark the check found no difference, and runs with 1 and 8 threads gave the same 182 game-state hashes.

## Limits

- Each timer reads a clock twice, so the timed phases run slower than they do with recording off.
  Compare runs with the same setting.
- The clock is `std::chrono::steady_clock`. On Windows it is the performance counter.
- Only the phases listed above are timed. Work outside them is not in any phase. To find it, subtract
  the child phases from their parent; `draw.total` also covers the screen copy before the present.

## Texture updates

The presenter keeps a copy of what each texture holds and compares the next picture with it in tiles of
32 by 16 pixels. It sends the tiles that differ, as rectangles, and sends nothing when no tile differs.
The frame, the map layer under it and the emissive masks all go this way. A texture that was just created
is sent whole, and so is a picture of which half or more has changed. `uploadverify 1` in an autotest
script checks every update against the picture and `uploadverify 0` writes the counts of checks and
mismatches.

## Scrolling a large map view

A scroll used to copy the whole tile layer and send most of the tile layer and the depth buffer to the
GPU again, because a strip one pixel wide touches every row. The tile layer now scrolls in place and a
pass sends only the rectangles the map drew ([GPU_MAP.md](GPU_MAP.md#uploads)).

Measured on a large Mental Omega mission map zoomed fully out at 3840x2160, a view of 7680x4320 map
pixels, with `GpuRender=yes` on a GeForce RTX 4090, scrolling 12 pixels sideways, 8 pixels down, or
both in every game frame:

| Scroll | Before | After |
|---|---|---|
| Sideways | 230 ms a frame | 12.7 ms |
| Down | 50 ms | 16.8 ms |
| Diagonal | 245 ms | 16.7 ms |

The same view standing still takes about 10 ms a frame. A full redraw of that view takes about 370 ms.

## Ambient light fades in a large map view

Each step of an ambient light change used to redraw the whole tile layer. The layer now keeps its picture
through the fade and the GPU scales it ([GPU_MAP.md](GPU_MAP.md#light-fades)), with one full redraw when the
light has settled and one whenever the picture would have to be brightened by more than half.

Measured on the same mission and view, with the light stepping one unit about every frame, as game time
per frame over the whole fade:

| Fade | Before | After |
|---|---|---|
| From 85 down to 55 | 103 ms a frame | 12 ms |
| From 55 up to 90 | 94 ms | 16 ms |
| The mission's opening fade from 0 to 85 | 200 ms | 45 ms |

The opening fade starts from black, where each step is a large change, so it still redraws the layer ten
times on the way up. A later fade to black and back keeps the bright picture and redraws nothing.

A light source that appears, moves, changes or goes, and radiation that lights the ground, used to redraw
the whole tile layer as well. They now redraw the part of the layer that shows the cells whose lighting
changed. An 800-frame stretch of the same mission with two fades to black and back and 137 light source
changes took 31 ms a game frame with the fades deferred alone and 14 ms with both, of which drawing the
tile layer takes 1 ms.

`FastestGpu` ([manual](../manual/content/keys/fastestgpu--client-settings.md)) matters for these numbers:
on a computer with two graphics cards the game could start on the slower one, where the GPU pass of this
view alone took about 31 ms.

## Zoom changes

A change of the zoom keeps the map's drawn picture and draws only the edges the old view did not show, as far
as the screen shows them: during a glide out, each draw adds the part of the new edges the glide has brought on
screen, with 64 map pixels around it for the GPU effects that read nearby pixels, and the rest is drawn when the
glide ends, at a scroll, while a light fade is deferred or at a full redraw ([Drawing the new edges](#drawing-the-new-edges)). A
step inward draws nothing new unless the map's edge moves the view. While a zoom glides, the map is drawn
every frame at the zoom the glide drew last, so objects keep moving, and the presenter scales the part of
that picture the glide shows. The GPU pass's scene target and the presenter's map texture keep the size of the whole drawn
view through a glide.

The map's two surfaces and its depth and alpha buffers are made once, at load, for the largest map view the player's zoom allows on that screen. By default that view is 2560x1440 map pixels, since `ZoomOutView` ([manual](../manual/content/keys/zoomoutview--client-settings.md)) limits the zoom out to what the original game shows at 2560x1440; `ZoomOutView=4K` makes it 3840x2160, and Debug mode in a single-player game lifts the limit to the range of 0.5. A scripted scene that zooms past the limit makes the surfaces again at its size. Each later change of the zoom uses part of
them: the tile layer, the depth buffer and the alpha buffer keep their contents where they are in memory,
and only the place where the view starts moves, as a scroll moves it. The part of the picture both views
show, with its emissive and shadow planes, is therefore neither copied nor sent to the GPU again; while the
GPU reads the tile layer in place, only the new edges of the three go up. The surfaces and buffers are made
again when the screen's map area changes size or place, and when the largest view grows past them, since
the zoom range follows the map's visible area. When the zoom returns to 1, the map surfaces are kept unused for the next zoom out, and
the depth and alpha buffers keep their size, until the screen's map area changes. The picture is drawn whole,
in the same buffers, while a light fade is deferred and with `OPENYR_NO_ZOOM_KEEP=1` in the environment.

Memory the map surfaces and the depth and alpha buffers use once the zoom has changed, as the `Map view` and
`Map surfaces` lines of the log give it. Before this change the same amount was used at the furthest zoom,
and less at a nearer one. Each emissive or shadow plane, kept while HDR highlights, `Bloom`, `SoftShadows`
or `WaterRipples` need them, adds one byte for each pixel of its surface's size.

| Screen, map | Largest view | Map surfaces | Depth and alpha | All |
|---|---|---|---|---|
| 1920x1080, XMP22S8 | 3840x2160 | 50 MB | 33 MB | 83 MB |
| 3840x2160, XMP04T8 | 7546x4245 | 199 MB, made for 7680x4320 | 128 MB | 327 MB |

These rows are the sizes at the furthest zoom before the zoom-out limit, which is what `DebugMode=yes` still gives. With the limit, the four buffers the first zoom change makes take these amounts (the `Map view` line of the log):

| Screen | Furthest zoom | Largest view | Buffers made | Before the limit |
|---|---|---|---|---|
| 1920x1080 | 0.75 | 2560x1440 | 37 MB | 83 MB |
| 2560x1440 | 1.0 | 2560x1440 | none | 147 MB |
| 3840x2160, scale 1 | none | 3840x2160 | none | 320 MB |
| 3840x2160, scale 2 (automatic) | 0.75 | 2560x1440 | 37 MB | 320 MB |

The tile layer takes two thirds of the map surfaces' share, because its memory holds twice its largest
picture for scrolling ([GPU_MAP.md](GPU_MAP.md#uploads)). At 3840x2160 the map surfaces were made at the
first zoom change of the game, when the largest view was 7680x4320, and were large enough for every later view.

The settled picture is the one a full redraw gives. `tests/autotest/zoom-glide.txt` glides ten mouse-wheel
steps out and ten back in on XMP22S8. Its `mapbuffers` lines show that the glide out changed the drawn zoom
five times and that only the first change made buffers. It logs the map digest of the settled view as left
and after a full redraw with `drawdigest 2`, which leaves out draws that repeat a frame's picture. The
number of draws in a game frame depends on the clock, so with every draw logged two runs gave different
frame and draw numbers for the same hashes; five runs now give the same log. At 1920x1080, window shots
taken during both glides and after them matched pixel for pixel the shots of a run with
`OPENYR_NO_ZOOM_KEEP=1`, in software and GPU drawing, and in GPU drawing also with `SoftShadows`, `Bloom`
and Enhanced HDR highlights on; the shots after the glides also matched with `RenderThread=yes`. The shots
after the glides matched those of the build before this change at 1920x1080 on XMP22S8 and at 3840x2160 on
XMP04T8.

Each step of the glide out, before and after, from one run each with the GPU busy with another program, so
the times are those of the processor only. Time is the zoom change itself in software drawing (making or
reusing the surfaces and buffers and moving the kept picture), as the `Map view` line gives it; drawing the
new edges is the same as before. Sent is what the GPU map pass sent of its textures in the passes that
followed the step, in GPU drawing.

| Screen, map | Step view | Made before | after | Time before | after | Sent before | after |
|---|---|---|---|---|---|---|---|
| 1920x1080, XMP22S8 | 2133x1200 | 4 buffers, 26 MB | 4 buffers, 83 MB | 6.2 ms | 13.5 ms | 27 MB | 91 MB |
| | 2400x1350 | 4, 32 MB | none | 8.5 ms | 0.3 ms | 33 MB | 5.1 MB |
| | 2742x1542 | 4, 42 MB | none | 10.8 ms | 0.4 ms | 43 MB | 6.9 MB |
| | 3200x1800 | 4, 58 MB | none | 15.3 ms | 0.5 ms | 59 MB | 9.9 MB |
| | 3840x2160 | 4, 83 MB | none | 19.7 ms | 0.8 ms | 84 MB | 15.7 MB |
| 3840x2160, XMP04T8 | 4266x2400 | 4, 102 MB | 2 buffers, 128 MB | 27.4 ms | 39.3 ms | 103 MB | 133 MB |
| | 4800x2700 | 4, 130 MB | none | 35.4 ms | 1.0 ms | 130 MB | 17 MB |
| | 5485x3085 | 4, 169 MB | none | 43.6 ms | 1.4 ms | 170 MB | 24 MB |
| | 6400x3600 | 4, 230 MB | none | 57.5 ms | 2.1 ms | 231 MB | 37 MB |
| | 7546x4245 | 4, 320 MB | none | 86.2 ms | 2.9 ms | 321 MB | 55 MB |

What is sent after a step is now the edges of the tile layer and of the depth and alpha buffers. A rectangle
that was named twice, as the strips a step brings in are by both the buffers and the background pass, goes
up once. The first step sends the whole of the new tile layer and buffers.

Some costs remain. In software drawing the presenter sends the whole map picture with every present, zoomed or not.
At 3840x2160 on XMP04T8 the view shot as the glide out left it differs from a run with `OPENYR_NO_ZOOM_KEEP=1` in 57
pixels, and the settled view after the glide in differs in 66 pixels even when it is redrawn whole; the build before this
change shows the same pictures as this one, and the cause is not yet known.

### GPU textures at a zoom step

A step used to make about ten GPU textures again when the drawn view changed size: the scene target, its depth
attachment, the post target, the readback, the snapshot, the two soft shadow masks, the tile shadow plane, the emissive
mask and the two glow pictures. In a ten-notch glide out at 3840x2160 that was 90 textures and 830 MB made, on ETOTAL with
`ZoomOutView=4K` and `InterfaceScale=2`. The first step also made the stores of the map surfaces and sent them whole, which
took about 42 ms to present and 35 ms to draw.

Now a texture is made when a pass first uses it. The soft shadow masks are made for the largest view the zoom allows and
used from their corner, so a step makes none of them. The scene target, the post target, the emissive mask target and the
depth attachments keep the exact size of the pass, because the presenter finds a map pixel by texture coordinate, and a
larger texture moves some pixels (a magnification of 1.5 puts a texel boundary on every third column). The same ten-notch
glide makes 10 textures (150 MB), one scene target for each size the view takes. The first step presents in about 16 ms and
draws in about 25 ms, since the map surfaces are made at load and kept as spares while the zoom is 1 (the depth and alpha
buffers are made at load for the largest view the limit allows), so the first wheel notch makes no buffer.

The remaining cost of a step was the software drawing of the edges the new view adds, which grew with each step from 4 to
29 ms of `draw.terrain` at the 3840x2160 view. [Drawing the new edges](#drawing-the-new-edges) spreads it over the glide.

`tests/autotest/zoom-glide.txt`-style glides of ten notches out on ETOTAL after its opening scene, one build before and one
after, two runs of each, with the machine busy (three to five other games and compilers). Worst frame and stall are from
the `perfwindow` line of the glide out, in ms; a run with another program's hitch is marked with an asterisk. Video memory
is the textures and render targets bgfx holds after the glide, in MB, and the textures made are those of the glide.

| Screen, `ZoomOutView` | Drawing | Worst frame before | after | Stall before | after | Memory before | after | Made before | after |
|---|---|---|---|---|---|---|---|---|---|
| 3840x2160, 4K | software | 51, 56 | 177*, 48 | 178, 216 | 855*, 158 | 196 | 196 | 20 | 20 |
| 3840x2160, 4K | GPU | 42, 174* | 32, 36 | 244, 2064* | 206, 206 | 406 | 275 | 101 | 11 |
| 3840x2160, 4K | GPU voxels | 58, 48 | 45, 49 | 263, 264 | 246, 242 | 573 | 450 | 131 | 51 |
| 3840x2160, Original | software | 154*, 28 | 27, 67* | 1112*, 135 | 151, 633* | 182 | 182 | 10 | 10 |
| 3840x2160, Original | GPU | 21, 22 | 16, 21 | 99, 142 | 88, 133 | 279 | 221 | 51 | 6 |
| 3840x2160, Original | GPU voxels | 27, 31 | 139*, 22 | 122, 134 | 1428*, 102 | 576 | 437 | 66 | 26 |
| 1920x1080, Original | software | 31, 32 | 30, 35 | 125, 117 | 134, 115 | 64 | 64 | 6 | 6 |
| 1920x1080, Original | GPU | 35, 28 | 27, 19 | 124, 119 | 99, 87 | 160 | 102 | 33 | 4 |
| 1920x1080, Original | GPU voxels | 41, 48 | 27, 23 | 137, 179 | 124, 132 | 289 | 234 | 42 | 16 |

The worst frame and the stall fall where the GPU draws, by about a fifth at 3840x2160 with the 4K limit and by more at
1920x1080, and stay within the spread of the runs elsewhere. A step costs about what it did, because the new edges take
most of it. The default view (the 2560x1440 limit) holds about 60 MB less video memory with GPU drawing than before, and
about 140 MB less with `GpuVoxels`. The render thread was off in these runs.

### Drawing the new edges

Measured on ETOTAL at 3840x2160 with `InterfaceScale=2` and `ZoomOutView=4K`, where a ten-notch glide out takes the view from
1920x1080 to 3840x2160 map pixels. Each step used to register the whole ring of new edges, four or five rectangles, as dirty
areas, and the next draw drew all of it through the same per-cell path a scroll strip takes. Cells that reach into the kept
picture are clipped to the ring, nothing is drawn twice, and the shroud and the depth and alpha buffers are reset only over the
ring. The cost was simply the size of the ring:

| Step view | New pixels | Cells drawn | Edge drawing, software | GPU drawing |
|---|---|---|---|---|
| 2021x1136 | 0.22 million | 412 | 3.4 ms | 3.2 ms |
| 2560x1440 | 0.45 million | 885 | 6.5 ms | 7.1 ms |
| 3200x1800 | 0.86 million | 1,397 | 8.9 ms | 10.9 ms |
| 3840x2160 | 1.44 million | 1,982 | 14.3 ms | 15.5 ms |

That is 5 to 8 microseconds a cell, about what a scroll strip costs (4.7 microseconds a cell for 12-pixel strips on a 1600x900
view). With GPU drawing about 40% of it is the first write to each memory page of the tile layer and the depth and alpha
buffers after the GPU pass took their written pages: the depth reset of the ring took 1.0 ms with GPU drawing and 0.1 ms in
software. A step also clears the ring in the depth and alpha buffers when it moves them, about 1 ms each with GPU drawing,
and the edges are cleared again when drawn.

A glide shows the new edges only gradually, so a draw now adds only the part of them on screen (`Tactical::Register_Undrawn_Edges`).
The largest edge drawing in one draw at the 3840x2160 view fell from 15.5 ms to about 7 ms; the total is the same, spread over
the frames the glide takes. Drawing more than is shown in each frame, up to 150,000 or 300,000 pixels, did not lower the worst
frames, because most of a step's edges come on screen in the two or three frames after its notch; it is not in the build.

The first step out from zoom 1 used to make the GPU texture of the map's tile layer (3840x4320 values) and send it whole, and
the step back to 1 made the screen tile layer's texture again; the presenter then took 10.6 ms. The GPU now keeps the
textures of both tile layers, makes the map's when the map surfaces are made at load, and sends only what was written to a
layer since it was last read ([GPU_MAP.md](GPU_MAP.md#uploads)); the first step presents in 3.7 ms. The spare map surface is
cleared when it is put aside at the end of a glide back to 1 instead of when the first step takes it. The first step still
copies the kept picture into the map's tile layer, about 2.8 ms.

Ten-notch glides out on ETOTAL after its opening scene (`InterfaceScale=Auto`, which is 2 here), two runs of the build before
and after, alternating, with one other game running in most runs and none of ours played; longest frame / total stall in ms
from the `perfwindow` line:

| `ZoomOutView` | Drawing | Before | After |
|---|---|---|---|
| 4K | GPU | 31.3 / 166, 30.3 / 149 | 24.5 / 216, 19.6 / 173 |
| 4K | software | 52.6 / 154, 45.6 / 157 | 38.6 / 166, 40.9 / 166 |
| Original (2560x1440) | GPU | 15.8 / 74, 16.8 / 86 | 15.9 / 93, 14.3 / 89 |
| Original (2560x1440) | software | 21.1 / 93, 19.5 / 78 | 19.4 / 78, 19.9 / 104 |

The longest frame falls by a quarter to a third at the 4K view; the stall does not, since the same edges are drawn and the
frames that draw them now lie above the median. The glide back in is unchanged. A settled frame at the 3840x2160 view takes
about 8 ms with GPU drawing, so the frames of a glide out, about 20 ms at its end, are still twice that; what remains is the
drawing of the edges the glide shows and the clearing of the depth and alpha buffers at each step.

We compared window shots of the build before and after at 21 frames of a ten-notch glide out and in (one, two or three
frames after a notch, and after the glide), as the frames left the view and after a full redraw, on XMP22S8 at 3840x2160
(`InterfaceScale=2`) and 1920x1080 with both `ZoomOutView` values, in software drawing, GPU drawing, and GPU drawing with
`GpuVoxels`, the render thread, `Bloom`, `SoftShadows` and `Shockwaves`; and on ETOTAL at 3840x2160 with `ZoomOutView=4K` in the
same three modes. All shots in software drawing on XMP22S8 are the same pixel for pixel. The others differ in at most a few
dozen pixels in one to five blocks of 16 by 16 pixels, in shots after a full redraw as well as in shots as left, and the
same differences appear between two runs of one build. `tests/autotest/zoom-glide-steps.txt` shoots the view after every
step for this comparison against a run with `OPENYR_NO_ZOOM_KEEP=1`.

We did not try showing the kept picture stretched while the edges are drawn later: such a frame would differ from a full
redraw at that frame's zoom.

### Soft shadows

`SoftShadows=yes` draws the shadows of the pass and the tile layer's shadow plane into a mask, blurs the mask along x and
then along y (radius 3 by default), and darkens the ground through it in the base pass. The mask is as large as the map
view, so the blur grows with the pixels of the view. Measured on the large battle of `bench-large-battle-xmp04-zoomed-out.txt`
at a 3840x2160 view (`InterfaceScale=2`, `ZoomOutView=4K`, frames 1,500 to 3,000), GPU time from bgfx's per-view timers on a
GeForce RTX 4090 and processor time from the Perf lines, in ms a pass or draw:

| Part | Before | After |
|---|---|---|
| Mask draw (GPU) | 0.028 | 0.028 |
| Blur along x and y (GPU) | 0.076 + 0.076 | 0.038 + 0.038 |
| Submitting the shadow draws (CPU) | 0.02 | 0.005 |
| Reading the shadow plane, `draw.gpumap` (CPU) | 0.33 | 0.02 |
| Recording the shadow blits, in `draw.objects` (CPU) | 0.2 | 0.2 |

All three passes run for every pass the GPU draws, whether or not a shadow changed. They run at the map's own pixels, whatever
the voxel scale. The cost with the setting on was therefore mostly on the processor: the shadow plane (one byte for each pixel
of the tile layer) was compared with its copy row by row whenever the tile layer changed. The plane now lives in
write-watched memory and only the rows its written pages touch are compared. The blur was already separable. Its loop runs
over the radius and no longer over 33 taps, and the mask draws of one run of shadow blits go in one draw call; the picture is
the same, because the taps are summed in the same order and every mask draw writes the same value.

We left out two cuts. Skipping the blur where the mask has no coverage needs the coverage of the tile layer's plane, and at
0.08 ms of GPU time on this card the bookkeeping would cost more processor time than it saves. Reusing the last blurred mask
is exact only while no shadow blit changed, and the mask also depends on the depth buffer, the shape atlas and the voxel parts,
so any moving object changes it.

An option that changes the picture, not shipped: blurring a half-size mask when the view is zoomed out would cut the blur to
a quarter of its pixels, but it moves the edge of every shadow by up to a pixel, and we have not measured its gain.

### Checking the pictures

We took window shots at the settled 1920x1080 view, mid-glide, settled after the glide out and after the glide back in, with
the glide result as left, after a full redraw and after setting the zoom directly in the same game frame, on a stock skirmish
(XMP04T8) and on ETOTAL, at the 3840x2160 and 2560x1440 views, with `GpuRender` yes and no, the render thread on and off,
`SoftShadows` on and off and `GpuVoxels` on and off. Apart from the sidebar, whose animation depends on the clock, every
shot of the skirmish equals the shot of the build before the change, pixel for pixel. The shots of ETOTAL differ in a few
dozen pixels in the same three or four blocks of 16 by 16 pixels, as two runs of the build before the change do. A glide's
result equals a full redraw and, apart from those blocks, setting the zoom directly.

The picture was first kept across zoom changes in an earlier change, which still made new surfaces and
buffers at each step and copied the kept part of the picture into them. Before that change, one step at
3840x2160 to a 3840x2160 map view took about 90 ms in both drawing modes: 14 ms for the two map surfaces,
9 ms for the depth and alpha buffers, 50 ms redrawing the tile layer and 8 to 15 ms presenting the new
picture, most of it in the GPU frame that takes the new textures. A step to the furthest zoom on XMP04T8 (a 7546x4245 view) took about 100, 250 and 85 ms for the
same three parts.

That change was measured with `zoom-glide.txt` played without the frame limit, before and after it, two
runs each, with no other game running except in eight of the 3840x2160 runs, which had one; the times are
the sum of the game frame intervals over each glide's 61 frames and the longest frame interval, in ms:

| Screen, map | Drawing | Glide out before | after | Longest frame before | after |
|---|---|---|---|---|---|
| 1920x1080, XMP22S8 | Software | 518, 541 | 307, 319 | 77, 80 | 52, 55 |
| 1920x1080, XMP22S8 | GPU | 300, 332 | 237, 249 | 83, 102 | 61, 72 |
| 1920x1080, XMP22S8 | GPU, `GpuVoxels=yes` | 402, 394 | 307, 265 | 92, 96 | 83, 69 |
| 3840x2160, XMP04T8 | Software | 2230, 2212 | 1094, 1144 | 370, 372 | 228, 244 |
| 3840x2160, XMP04T8 | GPU | 1383, 1406 | 922, 927 | 413, 446 | 276, 278 |
| 3840x2160, XMP04T8 | GPU, `GpuVoxels=yes` | 1727, 1646 | 976, 933 | 411, 444 | 292, 275 |

## Start-up with a large mod

Measured with Mental Omega (790 loose maps, rules of about 2 MB) to the title menu with the menu battle on, on
a computer that had read the files before:

| Step | Before | After |
|---|---|---|
| To the title menu | 6.2 s | 4.6 s |

On a first start after the computer was switched on the same build took about 20 seconds, most of it reading
the head of every loose map from the disk; that reading is what the first item below removes.

- With a mod active, the only time loose `.MAP` files are listed, the name and flags of each are kept in `mapinfo.cache` in the player's folder, with the
  file's size and time of last change. A file that changed, or a new one, is read as before.
- The outcome of each INI check is kept in `inicheck.cache` there, with a signature of the catalog and of every
  text the check read. An unchanged file gets one line in the log with its counts and its findings are not
  listed again; delete the file to have them listed. A test run always lists them, unless `[IniCheck]
  KeepInTests=yes` is set.
- The menu battle picks its map by trying maps in a shuffled order and reading each as it is tried, where it
  used to read the start positions of every listed map first; the count of each map is kept for the session.
- The loading notice names the part of the start-up in progress and shows a bar.

What remains is reading the rules for the menu battle (about 1.6 s) and its tile sets (0.7 s).

## Start-up time and file reads

From `WinMain` until the title menu stands or a game starts, the debug log gets `STARTUP` lines (`code/startuptrace.h`):
one per start-up step with the time since the process started, the megabytes read so far and the files opened so far,
one per archive mounted or cached and per INI file read, with its time and size, and a summary when the title menu
stands. The summary gives the opens, probes, reads and bytes of the file layer, the distinct bytes, the process's own
read counters, the fifteen files read most, the counts by file type and by archive member type, and a `STARTUP order:`
line listing the steps in order. After the summary the trace costs one flag test per file operation.

The "cold disk" line of the summary estimates what a first start after switching the computer on has to read: the
distinct bytes read through file handles, the pages of mapped archive members that were read, and one seek per file
opened, per seek that moves and per view. We measure this way because emptying the system file cache needs
administrator rights.

Measured to the title menu in test runs at 800x600 with two other test games running, with the INI check outcome kept
as in a normal start. "Battle" is the menu battle on a fixed map; the plain game's battle map is `XMP29U2.MAP`.

| Start | First frame | Loading notice | Title menu | MB read | Files opened | Cold disk |
|---|---|---|---|---|---|---|
| Mental Omega, battle, before | 1.02 s | 1.52 s | 5.97 s | 449 | 5,519 | 432 MB, 10,695 seeks |
| Mental Omega, battle, after | 0.44 s | 0.74 s | 2.47 s | 103 | 229 | 165 MB, 2,415 seeks |
| Mental Omega, before | 1.03 s | 1.52 s | 2.30 s | 73 | 189 | 64 MB, 246 seeks |
| Mental Omega, after | 0.38 s | 0.64 s | 1.01 s | 66 | 122 | 63 MB, 153 seeks |
| Plain game, battle, before | 1.06 s | 1.39 s | 2.77 s | 107 | 1,308 | 98 MB, 2,464 seeks |
| Plain game, battle, after | 0.37 s | 0.60 s | 1.07 s | 64 | 217 | 76 MB, 941 seeks |
| Plain game, before | 1.05 s | 1.37 s | 1.68 s | 63 | 313 | 57 MB, 525 seeks |
| Plain game, after | 0.37 s | 0.61 s | 0.81 s | 59 | 120 | 58 MB, 348 seeks |

- A member of an archive that keeps its members in memory, such as a mod's archives, was read whole on its first
  `Retrieve`. A member of 256 KB or more is now a copy-on-write view of the archive file, so only the pages that are
  read come from the disk, and a smaller one is copied out of a view without a file handle of its own. With Mental
  Omega the menu battle retrieved about 3,000 shapes, 280 MB, and read little more than their headers.
- A member of 1 MB or less of an archive on disk, other than a nested archive, is opened as a view of the archive file
  instead of opening the archive file again. With Mental Omega this took the voxel loads of the menu battle from about
  2,000 opens of the archive to none.
- The crash reporter loads the symbol file, about 120 MB for a release build with its symbols, on a thread of its own. It took half a
  second before the window opened. A crash report waits up to five seconds for it.
- File probes are answered from folder listings (`FileProbeScope`) while a map is read as well as while its rules are,
  while the scores are looked for and while the scenario list is built. With Mental Omega, 134 scores were each looked
  for under every sound file extension in every search folder.
- A loose map file is no longer looked for again when the scenario list is built, and the string tables are read whole
  before they are parsed instead of a few bytes at a time.

The `typesread` checksums of the menu battle's rules are the same before and after, the multiplayer `Sync` checksum among
them, for the plain game and for Mental Omega; the Rules, BulletTypes and All checksums differ between two runs of one
build and are left out. The string table counts, the scores found and the scenario list did not change.
`tests/autotest/startup-marks.txt` and `tests/autotest/mo-startup-marks.txt` check the step order and these checksums,
and the `mixmembers` unit test checks that a member reads the same through a view as through a file handle.

What is left: the renderer's start, about 0.3 s before the first frame; the four main archives and the theater
archives, about 80 MB, which are read whole because their digest is checked; and with Mental Omega, the menu battle's
"Reading the rules" step, about 1 s of processor time, mostly reading the types and their voxel models.

## Reading the rules

The "Reading the rules..." step of a scenario load builds every type again from the rules, the art and the
map. Measured on the step alone in test runs, while other test games ran on the same computer:

| Load | Before | After |
|---|---|---|
| Mental Omega, FCAPSULE, first load of the run | 2.91 s | 1.42 s |
| Mental Omega, FCAPSULE, restarted | 1.63 s | 0.99 s |
| Stock campaign ALL01UMD, first load | 0.60 s | 0.26 s |
| Stock campaign ALL01UMD, restarted | 0.27 s | 0.19 s |
| Stock skirmish | 0.50 s | 0.22 s |

- Most of the art names the types ask for (voxel turrets and barrels, cameos) are in no folder, and each probe
  of a folder used to try to open the file. While the step runs, a probe for a name a folder does not hold is
  answered from one listing of that folder (`FileProbeScope`). The saved-game load does the same in its rules
  step. With Mental Omega this took the probes of a restarted mission (about 24,000) from 0.36 s to 0.08 s, and
  those of a first load (about 70,000) from 1.17 s to 0.23 s.
- A warhead used to look up four `Versus.` keys for every armor type, which with Mental Omega's armor types is
  most of the time spent on warheads, and the warheads are read twice. A warhead now looks up only the armor
  types its own `Versus.` keys name: 0.20 s and 0.19 s became 0.03 s each.

Every type comes out the same. The `typesread` test command checksums the rules and every type list after the
step, both as a save holds them and as the multiplayer checksum covers them. For each load above the multiplayer
checksum was the same before and after, and the saved form of every type was the same byte for byte except for
members no code sets: the wave colour of a weapon without one (now zero) and a few bytes of the projectile types
and the rules, which also differ between two runs of one build. In the test runs the INI check of the map lists its findings on every load,
about 0.2 s of the Mental Omega step that a normal game skips for a map it checked before. Building the voxel
models of the vehicle types (0.2 s with Mental Omega) is now the largest part left.

## First frame after a load

The first frame draws the visible part of the map once, at the zoom the load leaves, and then once more when a mission's opening
sets the camera zoom. The Mental Omega mission SHAND once showed a 6 s main-thread stall that the hang watchdog pinned on the
first tile draw. The draw took about 14 ms. The 6 s was the load, which pumps no messages between its steps: the watchdog
allows a load 60 s, but the allowance ended with the load and the 6 s since the last pump was held against the first frame. The
watchdog now counts the end of a load as a pump, so the wait starts again there (`tests/autotest/hang-watchdog-load-end.txt`).

The tile layer of the first full draw, measured with counters in `Tactical::Draw_Tiles` and the `Render_Tiles` call, on a computer
running other test games, `InterfaceScale=Auto`, `GpuRender=yes`, `ZoomOutView=Original`:

| Mission | 640x480 | 1920x1080 | 3840x2160 |
|---|---|---|---|
| ALL01UMD (stock) | 379 cells, 3.0 ms | 2,521 cells, 11.7 ms | 9,420 cells, 51.4 ms |
| SHAND | 380 cells, 3.9 ms | 2,373 cells, 14.2 ms | 9,353 cells, 53.2 ms |
| ETOTAL | 359 cells, 1.6 ms | 2,330 cells, 11.9 ms | 9,202 cells, 48.9 ms |
| EMIGDAL | 425 cells, 4.8 ms | 2,387 cells, 13.4 ms | 9,420 cells, 69.8 ms |
| EGODSEND | 380 cells, 1.8 ms | 2,372 cells, 9.1 ms | 9,385 cells, 50.9 ms |

- The first draw covers the view only, and every tile it draws is read already: no tile art is loaded during it. The scenario
  load reads the tile art (529 tile types on SHAND, in the load's own step).
- The Mental Omega missions then draw a second time, 1,620 to 1,700 cells in 6 to 10 ms (1,230 to 1,250 cells at 640x480), after
  the opening's camera zoom changes the view. In the stock mission the next two draws were partial, 193 and 205 cells. The first draw cannot be skipped, since the zoom comes from a trigger that runs in the first game frame, after it.
- `GpuRender` and the two `ZoomOutView` values change the cell counts by none and the tile time by less than 2 ms. The zoom
  work (the fixed buffers, the zoom-out limit, the scaled selection list) did not touch `Draw_Tiles` or `Render_Tiles`, so the
  first frame draws what it drew before.
- From "game starts" to the first game frame SHAND takes 0.5 to 0.8 s at every window size and the stock mission 0.03 to 0.2 s.
  The window size does not change it, so it is the work of the first game frame, mostly its triggers, and not drawing.

The `tilecells` autotest step logs the cells the tile layer has drawn so far, and `tests/autotest/first-frame-cells.txt` logs
the count after the first draw of ALL01UMD at 1920x1080 (2,521) and 30 frames on, so a second or a larger first draw shows in a log.

## Voxel images

A voxel image is rendered once, kept run-length encoded in `VoxelStaticBuffer` (8,000,000 bytes) and
blitted from there on every later draw. A vehicle's hull is keyed by its facing, ramp and frame. In gamemd
its turret and barrel use the same kind of cache (`UnitClass::DrawAsVXL`, 0x73B470, through
`TechnoClass::Draw_Voxel`, 0x706640, with `VoxelTurretWeaponCache` and `VoxelTurretBarrelCache`).

Our turret and barrel keys are stricter than gamemd's, so a cached image is the one the draw matrix would
render: the hull's pose key, the hull facing, the turret facing, the turret frame or barrel pitch, and the
weapon slot of a multi-turret vehicle. A turret and a barrel each have a cache of their own
(`AuxVoxelIndex` and `AuxVoxel2Index`), so a barrel key never meets a hull key. A building's turret and
barrel are keyed by facing, frame and pitch, and only while both recoil offsets are zero. When the buffer
is full every cache is cleared, as before.

A turreted vehicle is assembled from its hull, turret and barrel on a 160 by 160 scratch surface and then
drawn to the map in one piece. The finished picture is cached too, keyed by the hull, turret and barrel
keys, and copied onto the scratch surface in place of the three part draws. A picture is kept only when it
lies wholly on the scratch surface and the buffer is less than half full, and these pictures are dropped
whenever any type's voxel images are cleared. The APC's water image, a `NoSpawnAlt` hull and types with
`DisableVoxelCache=yes` are not cached this way. In the first 1,500 frames of the large battle the map
draws matched hash for hash with and without this cache. At 3840x2160 with `GpuRender` and the frame rate
uncapped, the short benchmark rose from about 400 to about 530 pictures per second.

A cached picture of a vehicle in the normal visual state, not sinking, not warping and not split across a bridge is blitted straight to the map without the scratch surface, with the same pixels, depth and emissive values. A picture wholly outside the clip, like a voxel image or shadow there, is not drawn at all. In the short benchmark at 3840x2160 the time per voxel unit fell by about a third. Rows of a blit that the GPU recorder holds are walked only when they have emissive values to clear.

A vehicle whose shadow and turret picture are both cached no longer builds its draw and shadow matrices: the locomotor reports the cache keys without them, and the matrices are built only for a missing image. The time per voxel unit fell by about 1,000 of about 5,500 processor cycles.

A turretless vehicle whose hull image is cached no longer builds its draw matrix either, and passes the draw an identity matrix that a cached image never reads. Per `Unit_Draw_Voxel` call the median fell from about 5,900 to about 5,100 processor cycles in three alternating pairs of a benchmark with 10 armoured personnel carriers and 10 harvesters.

An aircraft body is cached as a vehicle hull is, keyed by the locomotor's key, the body frame and whether the aircraft is airborne (the locomotor's key does not carry that). A rocking aircraft, a dropship, a sinking one, a missile and a body frame of 32 or more still draw uncached. With 20 Orcas flying over the 4K view the `objects` phase fell from about 0.21 to about 0.07-0.12 ms and the `voxels` phase from about 0.06 to 0.00-0.01 ms in three alternating pairs. The draw digests of the run match with the cache on and off, in software and with `GpuRender`.

The presenter compares the frame's emissive mask with its copy only in rows that the mask's `Lit` flags say may hold a value, now or at the previous compare. Every other row is zero in both, so the copy already holds it. Where no row is lit, which is the usual case with `GpuRender`, the compare reads nothing. In three clean alternating pairs of the 4K benchmark `present.upload` fell from about 0.29 to about 0.21 ms and `present.total` from about 0.63 to about 0.50 ms.

The modern build panel is drawn eight pixels at a time. At 3840x2160 the `ui` phase of a picture fell from about 0.45 ms to about 0.09 ms with this change and the write watch change below.

The frame surface's write watch (`WriteWatch_Take` with `steady`) stops resetting a page that two takes in a row found written, because the next write to a reset page is a page fault of about 1,700 cycles. Such a page is reported as written in every take until a full reset, every 32 takes, so the areas redrawn every frame, such as the build panel, cost no faults, and a page that stops being written is still refilled and copied for up to 32 pictures. Only the frame surface uses it. In four alternating pairs of the 4K benchmark the draw phase fell from about 1.45 ms to about 1.1 ms per picture in quiet pairs.

Type tests on the simulation's hot paths use `Fetch_RTTI()` or the exact locomotor type instead of `dynamic_cast`: `Is_Foot`, the `As_...Class` casts, the occupier loop of the jumpjet cloak shimmer, the jumpjet and teleport locomotor tests, and the building test in `What_Weapon_Should_I_Use`. The C++ cast helpers took about 14.6% of the late-game benchmark's samples before and about 0.2% after. In three runs of each benchmark, `frame.logic` went from a mean of 7.08 ms (p95 22.2, worst 429) to 5.62 ms (p95 17.3, worst 333) in the late game, and from 1.03 ms (p95 2.26, worst 18.6) to 0.72 ms (p95 1.47, worst 10.7) in the large battle. A build configured with `-DOPENTS_RTTI_CROSSCHECK=ON` (and every Debug build) compares each replaced test with the `dynamic_cast` and counts the calls in `rtti_crosscheck.txt`. Over one late-game and one large-battle run it made about 330 million and 82 million comparisons, with no disagreement.

## Object removal broadcast

`Detach_This_From_All` has a `detach` frame phase and `detach calls`, `detach visits` and per-kind `detach` counters in the Perf lines. In `bench-mission-all02.txt` (ALL02UMD) a second holds about 260 broadcasts (121 animations, 64 bullets, 51 particles, 18 units, infantry, aircraft or buildings) and 394,000 `Detach` visits, which cost 0.17 ms per frame on average (p95 0.91 ms), 7% of logic. In `bench-bigmap-crossing.txt` (XMP22S8, one computer player) a second holds about 170 broadcasts and 201,000 visits, 0.035 ms per frame, 1% of logic. Bullets and particles cause most of the visits, because only animations have a short tracker of their own. Two kinds of removal now skip holders that cannot name the removed object:

- **An animation nobody holds.** `AnimPtrTracker` lists every building, bullet and super weapon. An animation is named by one of them only after `AnimClass::Held` marked it, which every store of an animation pointer in those classes goes through (building animations and damage fires, the nuke warning, a bullet's next animation, the chrono placement marker, the drain animation). The mark remembers the object that stored the animation. The removal of an unmarked animation visits none of them, and the removal of one stored by a single object visits only that object, if it still exists. An animation stored by two different objects, and every loaded animation, visits them all.
- **A bullet or a particle.** Units, infantry, aircraft, buildings, terrain objects, particles and bullets have no member that can point at one, so their `Detach` is not called for it; an animation is skipped for a particle only. Particle systems, waves, alpha shapes, houses, teams and factories are still visited. The kind of each holder in `ObjectPtrTracker` is kept in a byte array beside it, read from the holder at the first removal that needs it, so a skipped holder is not read at all. Every addition to and removal from `ObjectPtrTracker` goes through `Track_Object_Holder` and `Untrack_Object_Holder`, which keep the array in step.

`OPENYR_DETACH_CHECK=1` visits every skipped holder as well, compares its bytes before and after, and logs `DETACH CHECK DIFFERENCE` for a holder the visit changed; `2` also stops the game there. With the check on, the late-game benchmark made 130 million skipped visits and the full regress ran, with no difference, and the benchmark's game-state hashes are the same as before the skips.

## Leaving a large mission

`Delete_All_Objects` deletes every object and type when the game exits, when a mission restarts, and before the next mission or a saved game loads. Each deletion goes through the [object removal broadcast](#object-removal-broadcast), so the cost grows with the square of the number of types. The Mental Omega Epsilon mission EMIGDAL has 2,966 triggers with 5,047 events and 14,792 actions, and 21,498 types in all. Of the 4.75 s from the quit line to the end of `Prog_End`, deleting the types took 4.46 s, most of it in visits to and from the trigger events and actions.

Measured from the `AUTOTEST quit` line at frame 1500 (`-Uncapped`, 1920x1080) to the end of the process, two runs of each:

| Map | Before | After |
|---|---|---|
| EMIGDAL (Mental Omega campaign) | 4.91 s, 4.90 s | 0.78 s, 0.79 s |
| XMP29U2 (stock skirmish) | 0.11 s, 0.11 s | 0.12 s, 0.10 s |

What the removal of a type or a trigger part visits now:

- Trigger events and actions are not in `AbstractTypePtrTracker`. The removal of a team type visits events and actions, and that of a trigger or tag type visits actions; no other type can be named by them.
- A trigger type no longer passes each removal on to its own events and actions, because they are visited from `EventActionPtrTracker` themselves.
- A trigger type deletes its own events and actions without a broadcast: only it and its own lists name them.
- Tag types, trigger types, animation types and tile sets have trackers of their own, visited only for the kinds of type their `Detach` can change anything for: a trigger type for the first two, an animation or weapon type for animation types, and a tile set or animation type for tile sets.
- While `Delete_All_Objects` deletes the types, the removal of a team, trigger or tag type visits the events and actions only if one of them named that type when the deletion of the types began.

What is deleted and the order of the deletions are the same as before. `OPENYR_DETACH_CHECK=2` visits every holder these rules skip and compares its bytes: on EMIGDAL, through a mission restart and the exit, it checked about 1.2 billion skipped visits and found no difference. The `trackers` autotest step checks that the trackers hold exactly the trigger types, tag types, events and actions, in `tests/campaign/restart-trackers.txt` and, with Mental Omega, `tests/autotest/mo-emigdal-restart-trackers.txt`, before and after a restart.

Of the 0.78 s left on EMIGDAL, deleting the types takes about 0.47 s, most of it for the trigger types (0.25 s) and the 2,392 team types (0.15 s), and deleting the units, buildings and bullets about 0.1 s.

## GPU pass fallbacks

With `GpuVoxels=yes`, a pass whose voxel layers do not fit the model texture is drawn in software after the GPU path has recorded it, so it costs both. On the Mental Omega mission ETOTAL at 3840x2160 about 330 voxel parts a pass needed 2.1 to 2.4 million voxels where the texture held 2,097,152, so every pass from about frame 1,300 was refused and walked its missing layers again: a frame took 71 ms at zoom 1.19 and about 88 ms at zoom 0.5, against about 21 and 29 ms with `GpuRender=no`. The texture now holds 8,388,608 voxels, a layer takes only its own voxels and not whole rows, and a refused pass is not walked again. The same run has no refused pass and takes about 9.7 and 17 ms a frame.

A map pass that holds a draw the GPU record cannot replay is drawn in software, and the `GPU_RENDER` line counts it under `fallback`. `OPENYR_GPU_WHY=1` logs, for one such pass in 150, a `GPU_WHY frame` line with the counts and up to 12 `GPU_WHY kind` lines: the call stack of a surface lock no recorded blit accounts for (kind 0) or of a blit whose blitter has no `Describe` (kind 1), as addresses relative to the module base, to look up in `Game.map` at base 0x400000. `GPU_WHY blitter` names the blitter class. Two such draws were found this way on a large Mental Omega mission and now go to the GPU: the searchlight's depth glow lines, recorded as a light of the line's strength, and the rank insignia, drawn with `RLEBlitTransXlat`, which lacked `Describe`. A third was the flying text (the credits and damage numbers over objects), printed with `BitFontClass::Print` straight into the surface: on the Mental Omega mission STROPH three of four passes fell back for it. Each run of set glyph pixels is now recorded as a fill, and `GPU_RENDER` shows no fallback there.

## Zoomed out on a large Mental Omega mission

We measured the Mental Omega mission ETOTAL at 3840x2160, revealed and zoomed as far out as the build then allowed (a 4500x2531 map view), from game frame 1,400 to 4,000 with no input, with `GpuRender=yes`, `RenderThread=yes`, `CloudShadows=yes`, Enhanced HDR highlights, `GpuVoxelSmooth=yes` and the frame limit off. A draw visited about 2,940 objects, recorded about 2,100 blits and 520 voxel parts, and drew one GPU pass, with no pass drawn in software. Where the draw time went before the changes below, from one run with temporary timers around each part and a sampling profiler (ms a draw):

| Part | `GpuVoxels=yes` | `GpuVoxels=no` | Grows with |
|---|---:|---:|---|
| Recording the objects (`draw.objects`) | 3.8 | 3.6 | Objects on the map, since every object is visited, and the pixels of the shapes drawn |
| of which clearing emissive values under shapes | 1.05 | 0.3 | Rows of each shape drawn |
| of which voxels drawn in software | - | 1.0 | Voxel bodies with no cached image |
| Submitting the GPU pass at the present | 2.6 | 1.2 | |
| of which uploading the emissive mask and building the voxel mask | 1.0 | - | Pixels of the map view |
| of which the draw calls | 0.7 | 0.5 | Blits and voxel parts |
| of which uploading the depth, alpha and blit pixel stores | 0.5 | 0.6 | Blit pixels and changed rows |
| of which the voxel parts (`GpuVoxel_Submit`) | 0.25 | - | Voxel parts |
| Reading the emissive mask of the map view | 0.96 | - | Pixels of the map view |
| Background pass (`draw.terrain` and the reset of the emissive mask) | 0.6 | 0.5 | Pixels of the map view |
| Interface, HUD and frame copy | 0.4 | 0.4 | Pixels of the screen |
| `draw.total` | 8.8 | 6.0 | |

The GPU itself took about 3 ms a frame with `GpuVoxels=yes` and 1.6 ms without, as `present.gputime` gives it.

Most of the work that grows with the pixels of the view was the emissive mask, which HDR highlights keep. The map view's mask is 7,680 bytes wide, the size its surface was made for, while the zoomed view uses 4,500 of them. A row was marked as possibly holding a value until a clear covered the whole row, so after the first frame every row counted as lit. Each pass then cleared all 11 MB, every shape cleared the mask under each of its rows, and the GPU pass compared all 11 MB and sent every changed row whole, about 5 MB a draw.

Five changes, none of which changes a pixel:

- **Lit columns.** Each lit row of an emissive mask keeps the range of columns that may hold a value. A clear that covers that range unlights the row, and a clear of a lit row writes only that range.
- **Clears that change nothing.** A shape drawn with an emissive strength of zero clears the mask under it only in rows whose lit columns overlap it and hold a value there.
- **Faster compares.** Rows of the mask and of the map picture are compared 64 and 16 bytes at a time (`Bytes_Equal`, `Bytes_Are_Zero` in `tilediff.h`); the 32-bit build's inlined `memcmp` compared 4 bytes at a time.
- **Mask tiles.** The GPU pass's emissive mask is compared in tiles of 32 by 16 pixels and only the changed tiles go up ([GPU_MAP.md](GPU_MAP.md#uploads)). Upload bytes fell from 5.2 MB to 1.7 MB a draw.
- **Joined draws.** Neighbouring blit quads that share a blend state are drawn in one draw call. No such quad tests or writes depth or stencil, and quads in one draw blend in their order.

We checked this with window shots taken at three zooms (4500x2531, 3840x2160 and 1920x1080 map views) and logged `drawdigest 2` on ETOTAL and on a stock skirmish, with `GpuRender` on and off, `GpuVoxels` on and off and the render thread on and off, for the build before the changes, after each change and after all of them. Two runs of the build before the changes already differ in a few small spots (a few dozen pixels in three to nine blocks of 16 by 16 pixels, on one structure on ETOTAL and on small objects in the skirmish), and the software map digests differ between such runs too. Every pixel the changed builds differ in lies in those blocks, and with `GpuRender` on the ETOTAL map digests match draw for draw. The GPU, zoom, upload and emissive autotests gave the same logs before and after.

Measured on ETOTAL at 3840x2160 with the settings above, from game frame 60 to 4,000 with no input (the mission's opening scene holds a 1600x900 view to about frame 1,300), three alternating runs of each without the frame limit and one with it, ms a draw unless marked:

| Map view, `GpuVoxels` | Draw avg before | after | p95 before | after | Upload a draw before | after | Drawn a second, limit on, before | after |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 3840x2160, yes | 8.3 | 5.8 | 12.1 | 8.0 | 3.0 MB | 1.1 MB | 115 | 144 |
| 3840x2160, no | 5.2 | 4.0 | 6.5 | 5.4 | 1.3 MB | 1.3 MB | 163 | 171 |
| 2560x1440, yes | 3.3 | 2.9 | 4.8 | 4.6 | 1.5 MB | 0.8 MB | | |
| 2560x1440, no | 2.3 | 2.1 | 3.7 | 3.1 | 0.9 MB | 0.9 MB | | |

The 2560x1440 rows are two runs each. Other games ran on the computer during some runs, and the runs of one setting spread by up to 2 ms. With the frame limit on, the game held 44 to 45 frames a second in all runs; the hidden test window does not wait for the display, so the drawn rate is not what a visible window shows. `present.gputime` did not fall. It read about the same before and after with the frame limit on and at 3840x2160, and rose from 3.3 to 5.0 ms in the unlimited runs at the 4500x2531 view; we have not found why.

What is left, in the order of the time it would save: recording the objects (3.5 ms), most of it the drawing of the objects in view ([The object walk](#the-object-walk-on-a-large-mental-omega-mission)); the voxel parts take about 0.5 ms of draw calls and 0.25 ms in `GpuVoxel_Submit`; reading the emissive mask still compares the whole view (0.7 ms) where the lit columns could narrow it; the pixel store of the blits (about 700 KB) is copied and sent again every pass (0.25 ms).

## The object walk on a large Mental Omega mission

Most of the time `Tactical::Draw_Objects` (`draw.objects`) takes on ETOTAL goes to drawing the objects in view; the objects out of view cost about 0.15 ms a draw. We measured this with the mission not revealed, from game frame 1,320 to 4,000 with no input, `GpuRender=yes` and `GpuVoxels=yes`, in a build with a processor-cycle timer around each step of each object and a sampling profiler that recorded the function and its callers about once a millisecond inside the walk. An object is in view when its centre is, in the edge band when only the band `Coord_To_Pixel` adds around the view takes it, and out of view otherwise. The counts are the same in every run.

| Objects | 3840x2160 view: count | ms a draw | µs an object | 2560x1440 view: count | ms a draw |
|---|---:|---:|---:|---:|---:|
| Vehicles in view | 206 | 1.2 to 1.5 | 5.7 to 7.2 | 110 | 0.58 |
| Animations in view | 298 | 0.77 to 0.98 | 2.6 to 3.3 | 175 | 0.42 |
| Soldiers in view | 261 | 0.57 to 0.73 | 2.2 to 2.8 | 169 | 0.33 |
| Building lights in view | 13 | 0.13 to 0.16 | 10 to 12 | | |
| Structures in view (their extras; the body is drawn by the background pass) | 217 | 0.10 to 0.12 | 0.5 | | |
| Everything in view | 1,009 | 2.7 to 3.5 | | 579 | 1.45 |
| Everything in the edge band | 213 | 0.30 to 0.37 | 1.4 to 1.7 | 239 | 0.32 |
| Everything out of view | 1,801 | 0.14 to 0.16 | 0.08 | 2,205 | 0.14 |
| Selection marks, health bars and pips (`Draw_Post_Render`) | | 0.10 to 0.14 | | | 0.06 |
| Walk, with the timers | 3,023 | 3.6 to 4.5 | | 3,023 | 2.2 |

An object out of view costs the layer loop, for vehicles, soldiers and aircraft a fog look-up and a `Destination_Coord` call whose result is discarded, and one `Coord_To_Pixel`; animations and structures also enter their `Render`, which returns at its own view test. A spatial index over the layers would save at most that 0.15 ms, so we did not build one. The ground layer's sort is a single pass of the game logic each game frame, not part of the draw.

Inside the walk at the 3840x2160 view, the profiler gave these shares:

| What | Share of the walk |
|---|---:|
| Emissive mask: clearing the values under each shape row by row (`Emissive_RLE_Row` with its depth test, the `Bytes_Are_Zero` row test), and copying and clearing it under turret composites (`Emissive_Copy_Rect`, `Emissive_Fill_Rect`) | 21 to 22% |
| Walking shapes for the GPU recorder (`Draw_Shape`, `RLE_Blit` set-up and row skips) | 19 to 21% |
| Recording blits and voxel parts (`Atlas_Image`, the prepared voxel look-up, list growth) | 17 to 18% |
| Screen position and depth of each object (`Coord_To_Pixel`, `Get_Z_Adjust`, `Get_Z_Fudge_Cliff`, `Get_Z_Fudge_Column`, cell heights), some of it the timers' own position test | about 20% |
| Voxel draw matrices | 4% |
| Fog and shroud look-ups | 1.4% |

The emissive values under a drawn shape are mostly set: in a test build that cleared only the columns from a row's first value to its last, that span covered nearly the whole shape and the time did not change. One change leaves every pixel as it was:

- **Emissive rows.** `Emissive_RLE_Row` indexes the mask, depth and depth-shape entries of a row directly when the row's depth entries do not run past the end of the depth buffer, instead of testing for that wrap at every pixel. A draw with strength zero skips the depth test where the value is already zero, since it would write zero over zero. The function's share of the walk fell from about 10.7% to 8.9%, about 0.08 ms a draw.

With `GpuVoxels=no`, the `voxels` phase takes about 1.2 ms a draw at the 3840x2160 view. About 4 voxel bodies a draw are drawn without the cache, and all of them are of one Mental Omega vehicle type that sets `DisableVoxelCache=yes`, which gamemd honours too; every other body was found in the cache or added to it, with about 0.2 misses a draw.

The change gave the same pictures as the build before it. We took window shots at the 3840x2160, 2560x1440 and 1920x1080 views and logged `drawdigest 2` on ETOTAL (from frame 1,400) and on the stock skirmish map XMP29U2 with three computer players, with `GpuRender` on and off, `GpuVoxels` on and off and the render thread on and off, twice with the build before the change and once with it. Two runs of the build before the change already differ, in the same 16 by 16 blocks every time: on ETOTAL in three blocks along the base of one structure at the 3840x2160 view (40 to 85 pixels, brightness steps of up to about 50), fewer at 2560x1440 and none at 1920x1080; on the skirmish in four to nine blocks around small objects in a town. Every block the changed build differs in is one of those blocks. With `GpuRender` on, the ETOTAL map digests matched draw for draw across all three runs, so there the run-to-run difference arises after the map is recorded; we have not found its cause. The game state hashes of the skirmish matched in all runs, while its map digests differ between any two runs from the second frame they log, in every mode, as do the software digests on ETOTAL in most draws. That fits a draw between two game frames placing moving objects part way by the clock; the window shots are drawn at the game frame itself. The GPU, emissive, upload, zoom, digest, camera-determinism and selection autotests gave the same logs before and after, except for counts that follow the number of draws (`uploadverify checks`, the frames `gpu-voxel-compare` samples), with no upload mismatch in either.

ETOTAL at 3840x2160, three alternating runs of each build without the frame limit and one with it, with one to seven other games running on the computer (ms a draw, mean of the runs):

| Map view, `GpuVoxels` | Draw avg before | after | p95 before | after | `draw.objects` before | after | Drawn a second before | after |
|---|---:|---:|---:|---:|---:|---:|---:|---:|
| 3840x2160, yes | 6.8 | 7.3 | 9.5 | 12.5 | 3.4 | 3.7 | 79 | 76 |
| 3840x2160, no | 4.2 | 6.3 | 5.5 | 11.5 | 2.8 | 3.9 | 110 | 79 |
| 2560x1440, yes | 4.4 | 3.9 | 7.2 | 5.8 | 2.4 | 2.1 | 100 | 107 |
| 2560x1440, no | 2.1 | 3.0 | 3.1 | 4.1 | 1.4 | 1.9 | 142 | 115 |

The runs of one setting spread by up to 4 ms a draw (3840x2160 without `GpuVoxels`: 4.2, 7.8 and 6.8 ms after the change), far more than the change, so these runs show the load on the computer rather than the change; the profiler share above is the measure of it. The quietest run of each setting gave 5.8 and 5.3 ms (3840x2160, `GpuVoxels=yes`, before and after), 4.0 and 4.2 ms (`GpuVoxels=no`), 3.3 and 3.1 ms and 2.1 and 1.8 ms at 2560x1440.

### The emissive mask on the GPU

A pass the GPU draws now lists its writes to the emissive mask, and the GPU makes them when it needs the mask ([EMISSIVE_MASK.md](EMISSIVE_MASK.md#writes-of-a-pass-the-gpu-draws)). This also covers the copy and the mark under turret composites, which are now two list entries. At the 3840x2160 view with `GpuVoxels=yes`, the emissive mask's share of the walk fell from 19 to 20% to under 1%, and the walk, timed in the profiling build from frame 1,700, from 2.4 to 2.8 ms to 1.7 to 2.1 ms a draw. With `GpuVoxels=no` the share fell from about 3.7% to 1.3%, and the walk from 2.5 to 2.7 ms to 2.3 to 2.5 ms; there the software voxels take about 40% of the walk.

Plain runs, three of each build alternating and one with the frame limit, with up to three other games and 3 to 22 build processes running (ms a draw, mean of the runs):

| Map view, `GpuVoxels` | Draw avg before | after | `draw.objects` before | after | Drawn a second before | after |
|---|---:|---:|---:|---:|---:|---:|
| 3840x2160, yes | 4.95 | 3.79 | 2.59 | 2.09 | 102 | 108 |
| 3840x2160, no | 3.61 | 3.43 | 2.49 | 2.33 | 127 | 127 |
| 2560x1440, yes | 2.67 | 2.53 | 1.64 | 1.55 | 140 | 133 |
| 2560x1440, no | 1.91 | 1.63 | 1.29 | 1.09 | 158 | 172 |

One run at 2560x1440 with `GpuVoxels=yes` after the change took 3.97 ms; the other two took 1.88 and 1.73 ms. With the frame limit, the draw took 4.59 and 3.05 ms at 3840x2160 and 2.76 and 1.92 ms at 2560x1440, before and after.

The change gave the same pictures as the build before it, checked as for the emissive rows above: window shots and `drawdigest 2` on ETOTAL and the XMP29U2 skirmish with `GpuRender` on and off, `GpuVoxels` on and off, the render thread on and off, `Bloom` on and off, `HDRHighlights` Classic and Enhanced, and `EmissiveDebugView` on. Every 16 by 16 block the changed build differs in is one where two runs of the build before it also differ. The `maskcheck` step found no differing list and no differing GPU mask on ETOTAL across the 3840x2160, 2560x1440 and 1920x1080 views and the zoom changes between them, and on the skirmish. The GPU, emissive, upload, zoom, digest, camera-determinism and selection autotests gave the same logs, except for the frames that `gpu-voxel-compare` and `target-line-gpu` sample and the `uploadverify checks` counts, which follow the number of draws.

What is left in the walk, in the order of the time it would save: the screen position and depth of each object (about 28% of the walk after the change), worked out again every draw although most objects did not move; we did not cache them, because the depth adjustment reads the cell, bridges, tunnels, overlays and the locomotor, so we could not prove when a cached value goes stale. Next is the GPU recorder's look-up of each shape in the atlas (about 5%), most of it comparing the shape's rows with the copy kept from when it was decoded, once a pass. Comparing the rows with `Bytes_Equal` instead of the inlined `std::memcmp` did not help: the rows are short, and the look-up's share rose to about 6%.

## Target scans

`TechnoClass::Passes_Before_Disguise_Roll` ran the weapon selection and `Can_Fire` first for every candidate, 111,000 times a second in the large battle, although most candidates fail a cheap test such as the type mask or the owner. The fire test now runs after every other rejection in that function, and the type mask test runs right after the limbo and strength test. Every test involved only reads (the fire test, the weapon choice, the line of fire, the range and the capture and parasite checks draw no random number and assign nothing), so the targets chosen and the random draws are the same. A shadow build that ran the old order beside the new one on every candidate and compared the result and the random draw count checked 12.6 million candidates of the 7200-frame large battle with no mismatch; it is not kept in the tree. In the large battle `frame.threat` fell from about 0.82 to about 0.08 ms a game frame and weapon selects from 111,000 to about 600 a second.

A map-wide scan (a hunt order) visits every object on the ground. In the late-game benchmark (7 computer houses, 1,519 objects) 30,588 such scans visited 29.3 million objects in three minutes. About 68% failed the movement zone test, 16% passed every test and cost about 3,000 cycles each (the fire test about 1,400, the weapon choice twice, the threat score), and the rest failed a cheaper test. The zone test now runs right after the limbo and type tests, so most rejected objects are not read any further. The distance to a candidate is measured only when the scan has a range limit, which a map-wide scan does not. The weapon choice made against a candidate by the rejection tests is handed to `Target_Threat` and not made again, and the bridge test in `Passes_After_Disguise_Roll` reads the two cells only when the two objects differ on a bridge. Every moved test only reads, so the targets chosen and the random draws are the same. A shadow build that ran the old rejection tests beside the new ones checked 44.4 million candidates in the late-game benchmark (30,588 full scans, 4.6 million handed-over weapon choices) and 12.8 million in the large battle (57,516 full scans) with no mismatch; it is not kept in the tree. In the late-game benchmark, mean `frame.threat` fell from about 0.95 to 0.85 ms, the worst frame from 207 to 180 ms, and weapon selects from 163,000 to 136,000 a second (two runs each).

## Game logic on a large Mental Omega mission

On the Mental Omega Epsilon mission ETOTAL, played without input to frame 6,000 at 1280x720 with `GpuRender=yes`, `GpuVoxels=no`, the frame limit off and the Perf lines on, a game frame's logic took 5.0 to 5.7 ms on average. About 2,800 objects run their AI each frame. Where it went, from the Perf lines and a sampling profiler that recorded the function and its caller about once a millisecond while logic ran:

| Phase | ms a frame | Share of logic | What spends it |
|---|---:|---:|---|
| `threat` | 1.6 to 1.9 | 32% | 31 target scans a frame walk about 21,000 ring cells; the walk itself is 1.4 to 1.7 ms of it |
| `building` | 1.7 to 1.9 | 33% | Base defences' target scans, and the count of soldiers overpowering a structure: about 12% of all logic, a pass over every unit, soldier and aircraft for each overpowerable structure each frame |
| `infantry`, `unit` | 1.1, 1.0 | 23%, 20% | Spread over movement, vision and weapon choice; no single function above 2% |
| `detach` | 0.8 | 16% | 39 removals a frame, mostly particles and bullets, each a walk over about 3,000 holders |
| `other` | 0.8 | 15% | Particle systems, bullets and animations |

Frames whose logic took over 30 ms were mostly not slow work. A per-frame check of the thread's processor time against the clock showed the game thread not running for most of such a frame while other programs used the machine. The frames that did that much work were one-offs: the mission start (reshrouding and revealing the map, 21 and 26 ms, and removing 4,975 overlay objects, 23 ms), a trigger that gives a group of units to another house (11 ms, and 535 target scans in that frame, 17 ms), and a time-freezing super weapon at about frame 1,010 whose blast reached several hundred objects (38 to 52 ms).

Four changes leave the game state the same:

- **Ring scans.** A ring cell with no occupier is no longer passed to `Evaluate_Cell`, which finds nothing there, and `Evaluate_Just_Cell` is called only for a computer house at a difficulty that destroys walls, the only case where it returns a value.
- **Overpowering soldiers.** `BuildingClass::Overpowerer_Count` walks `Infantry` instead of `Technos`. Only soldiers overpower, and the two lists hold the same soldiers whenever the count is taken.
- **Removal broadcast.** The kind of each holder is kept beside `ObjectPtrTracker` ([Object removal broadcast](#object-removal-broadcast)). While recording, the visit count used to take a second walk over every holder, which the `detach` phase and the overlay's logic time included; the walk now counts its own visits.
- **Slow frame counts.** The `logic over 20 ms`, `logic over 33 ms` and `logic over 50 ms` counters ([Perf lines](#perf-lines)).

Each change was compared with the build before it on ETOTAL and EMIGDAL to frame 6,000 (a `hash` every 30 frames, 200 lines each) and on the `camera-determinism-still.txt` skirmish (35 lines): every line was the same. During development the ring scan ran the old walk beside the new one, with the random-number state put back between them: 186,000 scans gave the same target, value and result. `OPENYR_DETACH_CHECK=1` over ETOTAL checked 230 million skipped visits with no difference, and a check of every cached holder kind against the holder found no difference.

Two batches of three alternating pairs of ETOTAL runs, before and after, with two to four other games running on the machine. The table gives the means of the second batch (ms; a frame count is per run of about 5,400 frames):

| | Logic average | p95 | Worst | Frames over 20 ms | over 33 ms | over 50 ms |
|---|---:|---:|---:|---:|---:|---:|
| Before | 5.7 | 10.8 | 48 | 4.3 | 1.7 | 0.3 |
| After | 4.0 | 8.4 | 53 | 3.0 | 1.7 | 0.7 |

In the first batch the averages were 5.5 and 4.1 ms; one of its pairs had other load in both runs and gave 108 and 69 frames over 20 ms. The worst frames are the one-off frames above and frames where the game thread did not run, so they did not change. `threat` fell from 1.95 to 1.34 ms, `building` from 1.89 to 1.09 ms and `detach` from 0.85 to 0.37 ms.

What is left: the ring walk itself (about 1 ms), `HouseClass::Battery_Affects` (2% of logic, a pass over the house's super weapons for each structure each frame), `HouseClass::Is_Being_Drained` (1%, a pass over every structure for each power plant), the `Map` cell lookup and `In_Radar`, which are not inlined (4%), the cross-cast to `IPiggyback` in `FootClass::AI` (1%), and the timers themselves: with recording on, the clock reads of the per-object and per-candidate scopes took about 9% of logic.
