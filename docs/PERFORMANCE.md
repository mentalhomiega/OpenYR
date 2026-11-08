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
how evenly the game is paced. A solo game at game speed 1 should read a mean of 16.667 ms. `cpu` is the
processor time (kernel and user, all threads) that the process used in the window, so the waits are
idle when it stays well below the window length.

`present.gputime` is the GPU time of the newest frame the GPU finished, from `bgfx::getStats`. bgfx
measures it a frame or more behind the present that records it, so read it as the GPU cost per present,
not as part of `present.total`. A present whose renderer gives no GPU timer adds zero.

The gap line has one sample per present that reaches the presenter: the time from that present's start
to the start of the next one. Its worst value is the longest interval between two presents in the second.

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

The `ring cells` counter is taken only when the cell walk runs. Each radar-map ring cell the walk visits adds one
counter increment while recording.

## Benchmark

`tests/autotest/bench-large-battle.txt` runs a fixed battle for 7200 game frames on XMP08T2 and then
quits. It places 336 objects on frame 10: the player and three computer houses each get 40
vehicles, 40 infantry and two aircraft, and eight destroyers sit on the lake. The houses are named
with `houseunit` by their position in the house list (`#1` to `#3`). From frame 600 on, the computer
houses hunt, the player moves its forces across the map, and focus-fire orders go out.
`bench-large-battle-zoomed-out.txt` is the same battle with the view set to `zoom 0.5`.
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

## Target scans

`TechnoClass::Passes_Before_Disguise_Roll` ran the weapon selection and `Can_Fire` first for every candidate, 111,000 times a second in the large battle, although most candidates fail a cheap test such as the type mask or the owner. The fire test now runs after every other rejection in that function, and the type mask test runs right after the limbo and strength test. Every test involved only reads (the fire test, the weapon choice, the line of fire, the range and the capture and parasite checks draw no random number and assign nothing), so the targets chosen and the random draws are the same. A shadow build that ran the old order beside the new one on every candidate and compared the result and the random draw count checked 12.6 million candidates of the 7200-frame large battle with no mismatch; it is not kept in the tree. In the large battle `frame.threat` fell from about 0.82 to about 0.08 ms a game frame and weapon selects from 111,000 to about 600 a second.

A map-wide scan (a hunt order) visits every object on the ground. In the late-game benchmark (7 computer houses, 1,519 objects) 30,588 such scans visited 29.3 million objects in three minutes. About 68% failed the movement zone test, 16% passed every test and cost about 3,000 cycles each (the fire test about 1,400, the weapon choice twice, the threat score), and the rest failed a cheaper test. The zone test now runs right after the limbo and type tests, so most rejected objects are not read any further. The distance to a candidate is measured only when the scan has a range limit, which a map-wide scan does not. The weapon choice made against a candidate by the rejection tests is handed to `Target_Threat` and not made again, and the bridge test in `Passes_After_Disguise_Roll` reads the two cells only when the two objects differ on a bridge. Every moved test only reads, so the targets chosen and the random draws are the same. A shadow build that ran the old rejection tests beside the new ones checked 44.4 million candidates in the late-game benchmark (30,588 full scans, 4.6 million handed-over weapon choices) and 12.8 million in the large battle (57,516 full scans) with no mismatch; it is not kept in the tree. In the late-game benchmark, mean `frame.threat` fell from about 0.95 to 0.85 ms, the worst frame from 207 to 180 ms, and weapon selects from 163,000 to 136,000 a second (two runs each).
