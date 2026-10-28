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
```

Each phase reads `average/p95/worst` in milliseconds over the samples taken in that second. The p95
is the nearest-rank 95th percentile. Counts are totals for the second.

`present.gputime` is the GPU time of the newest frame the GPU finished, from `bgfx::getStats`. bgfx
measures it a frame or more behind the present that records it, so read it as the GPU cost per present,
not as part of `present.total`. A present whose renderer gives no GPU timer adds zero.

The gap line has one sample per present that reaches the presenter: the time from that present's start
to the start of the next one. Its worst value is the longest interval between two presents in the second.

## Skipped draws

With `RenderFrameRate` above zero, the wait between two game frames redraws the view each
`1000 / RenderFrameRate` ms. A due redraw is skipped when less time is left in the wait than the last
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
| `ring empty` | Of those, cells with no `TechnoClass` in either occupier chain (the bridge chain and the ground chain) |
| `ring wall` | Of those, cells with a wall overlay (`OverlayTypes[...]->IsWall`) |
| `cell candidates` | `Evaluate_Cell` calls that found an occupant and passed it to `Evaluate_Object` |
| `object evals` | `Evaluate_Object` calls, from the ring walk and from full-map scans |
| `area modify` | `Area_Modify` calls, including the calls that return 1 at once for a weapon that is not suppressive |
| `weapon selects` | `What_Weapon_Should_I_Use` calls |
| `ring keys`, `ring key repeats` | Ring walks with a distinct key and with a key an earlier walk in the same game frame already had. The key is the origin cell, the scanner's house, its type, its zone, the threat method and `crange`. It leaves out the exact origin coordinate, so a repeat can still find a different result |
| `draws skipped` | Redraws that were due in the wait between game frames but had too little time left. See [Skipped draws](#skipped-draws) |

The ring counters are taken only when the cell walk runs; they add a read of each ring cell while recording. The
`ring keys` counts use a set cleared at each game frame, so they cost more than the other counters; expect the
scan time to grow while recording.

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
