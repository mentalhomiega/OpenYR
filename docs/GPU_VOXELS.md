# GPU voxel drawing

Vehicles, aircraft and other voxel objects are drawn by the software voxel renderer, which projects each voxel
model at one of 32 prepared facings and caches the picture. We are moving that drawing onto the GPU in stages, so
that objects can turn smoothly and stay sharp at high resolution. [GPU_MAP.md](GPU_MAP.md) describes the GPU map
pass this builds on.

## Where we are

Stage 1 is in. With `GpuVoxels=yes` in `[Video]` of `RA2MD.INI` (default `no`), a voxel object that is blitted
straight into the map view is drawn on the GPU from its voxels instead of from the software's 256 by 256 picture.
The GPU result is the same picture, index for index; nothing is smoother or sharper yet. The key does nothing
unless the GPU draws the pass (`GpuRender`) or the `gpucheck` autotest step runs, and the Graphics page has no
switch for it, because stage 1 has no visible benefit to offer.

Stage 1 draws these objects on the GPU:

- vehicles and aircraft with no turret (hull only), such as an MCV, a Kirov or an Orca;
- nothing else. Turreted vehicles compose hull, turret and barrel on an 8-bit scratch surface, which stays in
  software, and so do shadows, voxel animations and bullets.

While the emissive mask is on (`HDRHighlights=Enhanced` with an HDR display, or `Bloom`), `GpuVoxels` draws nothing,
because the software voxel drawers write the mask and the GPU drawing does not. A voxel object drawn this way never
uses the voxel picture cache.

Stage 5 is in too. With `GpuVoxelSmooth=yes` (`[Video]`, default `no`, needs `GpuVoxels=yes`), a hull-only object the GPU
draws turns to its exact facing: `LocomotionClass::Draw_Body_Radian` (`code/loco.cpp`) gives the matrix `DirType::As_Radian_Smooth`
instead of `As_Radian32`, which is the same number at each of the 32 facings. `FacingClass::Current` already reads the facing part
way between two game frames when `RenderFrameRate` is above the game rate (`RenderBlend`), taking the short way round, so the
smooth angle follows it. A turreted vehicle keeps the 32 facings until stage 2, because its turret and barrel are composed from the
hull's matrix on the scratch surface, and shadows keep their cache key, so they still move in steps (stage 4). With the key off,
nothing changes.

## How the software draws a voxel object

`TechnoClass::Draw_Voxel` (`code/techno.cpp`) draws one model, a hull, a turret or a barrel, at a time.

1. **Lighting table.** `Precalculate_Normal_Lookup` (`code/voxlib.cpp`) turns each normal of the first layer's
   normal table into a palette row, 0 to 31: the dot product of the normal with the light vector, taken into the
   object's space through the object's matrix, plus an optional specular term, scaled by `VoxelShadeScale`. One
   table serves every layer of the object. Normals 253 to 255 are always the neutral row 16.
2. **Layers.** Each layer of the VXL gets the object's matrix times the HVA matrix of the current frame.
   `Prep_For_Object` transforms the eight corners of the layer's box into view space (the camera matrix is the
   isometric view) and takes the corner nearest the viewer as the anchor. The object's layers are drawn in order of
   their far Z, farthest first.
3. **Projection.** `VoxelLibrary::Setup_Object` turns the anchor corner and its three neighbours into a 16-bit
   fixed point origin and three steps, one for each voxel along the layer's X, Y and Z, in 1/256 pixels. The anchor
   picks one of eight walk orders (`VoxelRenderOrientations`): columns run row by row in the direction away from the
   viewer and each column runs from the far end, so nearer voxels are written later. The positions are 16-bit
   sums that wrap, and each step is truncated once, so the picture depends on that arithmetic.
4. **Drawing.** The drawers (`Draw_Voxel_*_Lighting` in `code/voxlib.cpp`) write each voxel's palette index into the
   256 by 256 byte voxel buffer at `(x >> 8) | (y & 0xFF00)`. A voxel with a normal writes two neighbouring
   pixels, one without writes one. The game runs with lighting on and the voxel Z buffer off
   (`VoxelDrawSystem::Disable_ZBuffer`), so a later voxel simply overwrites an earlier one. The lit index is
   `VoxelPaletteTranslateTable[row][colour]`.
5. **Blit.** The part of the buffer that the object covers, plus a 4-pixel margin, is blitted to the map view
   through the owner's colour scheme converter: house remap, brightness (`Apparent_Brightness`), translucency and
   alpha lighting from the shape flags. The blit tests depth against the map's depth buffer with a per-row gradient
   (`Get_Z_Adjust`, `Get_Z_Gradient`) and writes no depth. This is the same blit the GPU map pass records for any
   shape.
6. **Caches.** A picture is cached per type and facing key (`VoxelIndexClass`, `VoxelStaticBuffer`), a turreted
   vehicle's composed picture is cached too, and the GPU keeps cached pictures in its image atlas. A shadow is
   drawn separately by `Render_Shadow`: the occupied columns of the layer, flattened onto the ground and slid along
   the light vector, blitted darkened.

## The GPU path

A part is one prepared object: its layers' `VoxelRenderStruct` copies, the projection centre, the lighting table and
the part of the voxel buffer it covers (`VoxelDrawSystem::Capture`, `GpuVoxelPart` in `code/gpudraw.h`).

- `Gpu_Voxel_Walk` (`code/gpuvoxel.cpp`) lists a layer's voxels in the order the drawer writes them, with their
  place in the walk, colour and normal. `tests/gpuvoxel` draws that list on the CPU the way the shader does and
  compares it with the software drawers for random layers in all eight orientations.
- `code/gpumapvoxel.cpp` keeps those lists in a 2048 by 2048 model texture, two texels a voxel, one list for each
  layer and orientation, and a 2048 by 2048 voxel image of palette indices. Each part gets a tile of the image the
  size of its buffer region. `vs_mapvoxel.sc` reads the voxel for each vertex from `gl_VertexID`, computes the same
  16-bit position, looks up the lit index and draws a one or two pixel quad into the tile; the order of the
  vertices is the order of the software's writes, and Direct3D keeps primitive order, so later voxels overwrite
  earlier ones as they do in software.
- The recorded blit has no pixels of its own. Its colour index offset points into the voxel image (`GPU_VOXEL_BASE`
  in `code/gpumapvoxel.h`), and `fs_mapdraw.sc` reads the index there and then does everything it does for any
  blit: translator, alpha lighting, depth test, blend kind.
- A pass the GPU cannot draw (a limit, a failed program) renders the parts in software when it falls back, so the
  picture is complete.

## Comparing the two

`tests/autotest/gpu-voxel-compare.txt` plays eight vehicle types turning through sixteen facings, with a Kirov and
an Orca overhead, under `gpucheck`, with `GpuRender=no` so the software draws the pass. Each voxel part drawn in a
checked pass, including the hull, turret and barrel parts that go to the scratch surface, is also drawn on the GPU
and read back, and the log gets one line:

`GPU_VOXEL frame F part TYPE@FACING layers N pixels P covered C differing D coverage V maxstep S`

`pixels` is the part's buffer region, `covered` the pixels either side wrote, `differing` the pixels whose palette
index differs, `coverage` those where only one side wrote a pixel, and `maxstep` the largest difference in a 565
channel between the voxel palette colours of the two indices. The pass's usual `GPU_DIFF` line then compares the
finished picture, where a direct voxel blit goes through the same shader as every other blit.
`GPU_RENDER` lines also count the voxel blits the GPU drew.

| Type | Parts compared | Facings | Covered pixels | Differing |
|---|---|---|---|---|
| MTNK | 174 | 16 | 62,935 | 0 |
| HTNK | 174 | 16 | 79,304 | 0 |
| APOC | 174 | 16 | 89,006 | 0 |
| TTNK | 116 | 16 | 80,399 | 0 |
| FV | 116 | 16 | 85,215 | 0 |
| SREF | 116 | 16 | 79,829 | 0 |
| HARV | 116 | 16 | 89,215 | 0 |
| AMCV | 59 | 16 | 116,617 | 0 |
| ZEP (Kirov) | 57 | 1 | 223,098 | 0 |
| ORCA | 57 | 1 | 32,091 | 0 |

The `GPU_DIFF` lines of the same run show the hundred or so one-step pixels that the run shows with
`GpuVoxels=no`; the hull-only MCV, Kirov and Orca blits add none. Rendering the
same scene with `GpuRender=yes` and `GpuVoxels` on and off gives the same screenshot outside the sidebar.

The numbers vary from run to run in which passes are read back, so the regress log of the script keeps only its
crash lines.

## Checking smooth facing

`tests/autotest/gpu-voxel-smooth.txt` turns two MCVs half a turn under `gpucheck` with `GpuVoxelSmooth=yes`. Each `GPU_VOXEL` line
labels its part `TYPE@F` with `F` the facing in 32nds of a turn: whole numbers with the key off, fractions such as 8.62 with it on,
rising in even steps with no repeats. The software and the GPU draw the same smooth matrix, so the compare stays at 0 differing pixels.

## Stages

Each stage has a check that fails when its picture departs from the software.

| Stage | What it adds | Check | Estimate |
|---|---|---|---|
| 1 (done) | Walk, model store, voxel image, shader, direct blits, fallback, `GPU_VOXEL` check | `gpuvoxel` unit test, `gpu-voxel-compare.txt`, zero differing pixels | |
| 2 | Turreted vehicles: draw hull, turret and barrel into one tile in the order the scratch surface composes them, and blit the tile like a direct part | `GPU_DIFF` over turreted units; screenshots with `GpuRender` on and off | 1 to 2 sessions |
| 3 | Emissive mask: write the highlight strength beside the index and feed the glow plane and the HDR composite from it, so `GpuVoxels` works under `Bloom` and HDR Enhanced | glow and HDR frames compared with the software mask | 1 session |
| 4 | The other voxel draws: shadows (the same walk of occupied columns, drawn darkened), voxel animations, bullets, debris and the translucent, predator and warp flags | `GPU_DIFF` on the existing voxel anim and cloak scripts | 1 session |
| 5 (done) | Smooth facing: build the object matrix from the exact facing instead of the 32-step key and draw every frame without the cache | the angle at each of the 32 keys equals the stage 1 angle (`shapefacing` test); `gpu-voxel-compare.txt` stays at 0 differing pixels; `gpu-voxel-smooth.txt` logs a turning MCV's facing in fractions of a step | |
| 6 | Sharpness: draw at a scale factor, each voxel as the silhouette of its cube instead of a one or two pixel quad, with lighting kept in palette rows (or in float behind a second key) | the scale 1 mode stays index-exact; larger scales are compared by eye and by edge counts | 2 sessions |
| 7 | Settings page switches (greyed unless GPU drawing is on), default on once stage 2 to 5 match, instance buffers and batching, performance numbers on the large battle | the regress suite with the key on | 1 session |

## Where pixel-exact drawing ends

- Stage 5 and 6 change the picture on purpose, so they can only be exact at the facing keys and at scale 1.
- The software writes a voxel's second pixel on the next buffer row when the first is in the last column; the GPU
  clips it. Objects sit near the buffer's centre, so no object we have drawn reaches that column.
- Hardware blending cannot reproduce the software's flooring of halved and quartered terms; this already limits the
  translucent blits ([GPU_MAP.md](GPU_MAP.md), Limits) and will limit translucent voxels in stage 4.
- The model texture holds about a million voxels' worth of walks (a layer is stored once for each of the eight
  orientations it is drawn in) and is emptied when full, so a pass with more distinct layers than that falls back
  to software. The voxel image holds the parts of one pass, several hundred of average size; a pass with more is drawn in software.
- A float lighting model (stage 6) cannot give the software's palette rows, so it is a separate look.
