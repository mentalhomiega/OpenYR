# GPU voxel drawing

Vehicles, aircraft and other voxel objects are drawn by the software voxel renderer, which projects each voxel
model at one of 32 prepared facings and caches the picture. We are moving that drawing onto the GPU in stages, so
that objects can turn smoothly and stay sharp at high resolution. [GPU_MAP.md](GPU_MAP.md) describes the GPU map
pass this builds on.

## Where we are

Stages 1, 2 and 4 are in. With `GpuVoxels=yes` in `[Video]` of `RA2MD.INI` (default `no`), a voxel object that is
blitted straight into the map view, or a vehicle composed from its hull, turret and barrel, is drawn on the GPU from
its voxels instead of from the software's 256 by 256 picture. The GPU result is the same picture, index for index;
nothing is smoother or sharper yet. The key does nothing unless the GPU draws the pass (`GpuRender`) or the
`gpucheck` autotest step runs, and the Graphics page has no switch for it, because these stages have no visible
benefit to offer.

These objects are drawn on the GPU:

- vehicles and aircraft with no turret (hull only), such as an MCV, a Kirov or an Orca;
- vehicles with a voxel turret and barrel, such as a Grizzly, a Rhino or an Apocalypse, with the turret at any
  facing and the barrel at any pitch;
- the shadows of those vehicles and of aircraft;
- voxel animations and their shadows, which includes voxel debris;
- voxel bullets, such as a V3 rocket or a Dreadnought missile.

The translucent and warp flags of a voxel object are drawn through the same translucent blends as any other blit, and
a predator draw through the plain translucent blend (see "Where pixel-exact drawing ends"). A turreted vehicle whose body is a shape stays in software (the scratch surface holds a sprite the GPU does not draw).

`GpuVoxelScale` (`[Video]`, 1 to 4, default 1, needs `GpuVoxels=yes`) is stage 6, described under Sharpness below.

A voxel object drawn this way never uses the voxel picture cache.

Stage 3 is in. While the emissive mask is on (`HDRHighlights=Enhanced` with an HDR display, or `Bloom`), the GPU also writes each
voxel's highlight strength beside its index and feeds the glow and the HDR picture from it; see Highlights below.

Stage 5 is in too. With `GpuVoxelSmooth=yes` (`[Video]`, default `no`, needs `GpuVoxels=yes`), an object the GPU draws turns to its
exact facing: `GpuDraw_Facing_Radian` (`code/gpudraw.cpp`) gives `DirType::As_Radian_Smooth` instead of `As_Radian32`, which is the
same number at each of the 32 facings. `LocomotionClass::Build_Draw_Matrix` uses it for the hull, and `UnitClass::Unit_Draw_Voxel`
for the turret, turned by the exact turret facing minus the exact hull facing so that the turret lands at its own facing on top of
the hull's. The barrel pitch stays in 32 steps. `FacingClass::Current` already reads the facing part way between two game frames when
`RenderFrameRate` is above the game rate (`RenderBlend`), taking the short way round, so the smooth angle follows it for the hull
and for the turret alike. A shadow turns the same way: `LocomotionClass::Build_Shadow_Matrix` and `FlyLocomotionClass::Build_Shadow_Matrix`
use `GpuDraw_Facing_Radian` too, so the shadow stays under its object. With the key off, nothing changes.

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
- A shadow is one flat layer. `Gpu_Voxel_Walk_Shadow` lists one voxel with K 0 and colour 1 for each occupied column,
  `VoxelLibrary::Setup_Shadow` gives the projection the software shadow drawer uses, and the shader writes two
  pixels a column through the identity palette row. A voxel animation's shadow is drawn the same way.
- The recorded blit has no pixels of its own. Its colour index offset points into the voxel image (`GPU_VOXEL_BASE`
  in `code/gpumapvoxel.h`), and `fs_mapdraw.sc` reads the index there and then does everything it does for any
  blit: translator, alpha lighting, depth test, blend kind.
- A turreted vehicle is one part with a piece for each of its hull, turret and barrel, drawn in the order the
  software draws them: the hull, then the barrel and the turret, the barrel before the turret at some turret
  facings and after it at the others. The software composes them on the 160 by 160 scratch surface, where a later
  piece replaces the pixels below it that it draws, and the scratch surface passes each pixel through the colour
  translation of its own drawer. The part's tile covers the scratch surface's dirty rectangle; each piece is
  drawn into the tile at the place it would land on the scratch surface, clipped to the tile, with the palette
  texture's second block, which is the first run through that translation. The blit that shows the composite
  reads the tile at the pixels it would read from the scratch surface, so the sorting, depth and lighting of the
  blit are the same as for a direct part.
- While the GPU draws a pass, the software renders none of a part's pieces and writes nothing to the scratch surface.
- A pass the GPU cannot draw (a limit, a failed program) renders the parts in software when it falls back, so the
  picture is complete.

## Sharpness (stage 6)

With `GpuVoxelScale` above 1, `GpuRender` draws the whole map pass at that many times the map's own pixels. The scene,
snapshot and post textures are scale times the area in each direction, and the pass's views are scaled the same way, so
every blit keeps its map pixel coordinates and the shader shows each map pixel as a block of scale by scale pixels.
The shadow mask, glow and tile shadow planes stay at map size; the post and glow shaders read the scene at the
matching finer pixel. `bgfxbackend.cpp` treats the texture as scale times larger, so the presenter smooths it when it is
shown smaller than that and keeps whole pixels when it is shown at that size or larger. Zooming in the map view (`viewzoom.cpp`)
therefore shows the finer picture.

A voxel part's tile in the voxel image is scale times its buffer region in each direction. A voxel is drawn as the
silhouette of its cube: the three faces that meet at the corner nearest the viewer (the one on the line of sight through
the cube, found from the signs of the cross products of the layer's three projected steps), 18 vertices a voxel, centred
on the point the software's quad is centred on, with the same palette row. Voxels are still written in the software's
order, so nearer voxels overwrite farther ones. Everything else about a voxel blit, translator, depth test and blend,
is unchanged.

Scale 1 takes the old path and stays index-exact. A pass that `gpucheck` reads back is drawn at scale 1, because the
software result it is compared with has no finer pixels. At a scale above 1, `tests/autotest/gpu-voxel-scale.txt` shows
two tanks and a Kirov at zoom 1, 2 and 3 for comparing scales by eye and by edge counts.

A shadow's column is drawn as its flat cell, the one face of the cube a zero Z step leaves, in the same tile as a
direct part, so a shadow stays in register with its object at any scale. With `SoftShadows`, the shadow reads its tile
for the map-sized mask.

The scale also applies to a turreted vehicle's parts: each piece's offset inside the tile and the clip that keeps it in its tile are scaled the same way, so hull, turret and barrel stay in register and, with `GpuVoxelSmooth=yes`, sharpen at their exact facings.

## Comparing the two

`tests/autotest/gpu-voxel-compare.txt` plays eight vehicle types turning through sixteen facings, with a Kirov
and an Orca overhead, then turns the turrets of five turreted types to facings other than the body's and lets two
tanks drive with their turrets turning, under `gpucheck`, with `GpuRender=no` so the software draws the pass, and
`HDRHighlights=Classic`. Each voxel part drawn in a checked pass, a composite
counting as one part, is also drawn on the GPU and read back, and the log gets one line:

`GPU_VOXEL frame F part TYPE@FACING layers N pixels P covered C differing D coverage V maxstep S`

`FACING` is the body's facing in 32 steps, followed by `t` and the turret's facing for a composite. `pixels` is the
part's tile, `covered` the pixels either side wrote, `differing` the pixels whose palette index differs,
`coverage` those where only one side wrote a pixel, and `maxstep` the largest difference in a 565 channel between
the voxel palette colours of the two indices. For a composite the software side is the scratch surface after
the pieces are composed. The pass's usual `GPU_DIFF` line then compares the finished picture, where a voxel blit
goes through the same shader as every other blit. `GPU_RENDER` lines also count the voxel blits the GPU drew.

| Type | Parts compared | Body facings | Turret facings at a body facing | Covered pixels | Differing |
|---|---|---|---|---|---|
| MTNK | 81 | 16 | 30 | 66,448 | 0 |
| HTNK | 81 | 16 | 28 | 83,714 | 0 |
| APOC | 81 | 16 | 29 | 94,853 | 0 |
| TTNK | 81 | 16 | 29 | 86,272 | 0 |
| FV | 81 | 16 | 29 | 101,867 | 0 |
| SREF | 81 | 16 | 16 | 102,449 | 0 |
| HARV | 81 | 16 | 16 | 113,168 | 0 |
| AMCV | 82 | 16 | 0 | 153,557 | 0 |
| ZEP (Kirov) | 81 | 1 | 0 | 317,034 | 0 |
| ORCA | 81 | 1 | 0 | 45,603 | 0 |

A part is compared at most once for each pass the check reads back, so the same facing appears in several
parts. Sixty-five of the 567 composites had a turret facing different from the body's. The `GPU_DIFF` lines of the
same run have the same one-step pixels, about three a pass, as the run with `GpuVoxels=no`; none of them lies on a
vehicle.

The numbers vary from run to run in which passes are read back, so the regress log of the script keeps only its
crash lines.

## Highlights (stage 3)

While the emissive mask is on, `vs_mapvoxel.sc` computes the strength the software drawers write beside each palette
index (`Emissive_Voxel_Highlight`, zero for an unlit layer), and `fs_mapvoxel.sc` writes it into the voxel image's green
channel, beside the index in its red channel.

A direct part's blit copies the mark `GPU_VOXEL_MASK_MARK` from the voxel mask buffer into the target's plane, over its
whole rectangle, where the software would have copied the part's mask. Every other draw in the pass writes the plane as
it always has, so a pixel still holding the mark at the end of the pass is one no later draw covered. The pass then builds
the finished mask (`fs_mapmask.sc`, `Submit_Voxel_Mask` in `code/gpumap.cpp`): a copy of the plane, then for each direct
part's blit, in order, the strength from the part's tile wherever the plane holds the mark. The glow reads this mask in
place of the plane, and the present draws it over the map area for the HDR composite. A pass the GPU cannot draw renders
its parts in software and replaces the marks with their real strengths.

A turreted composite writes no mask, as in the software: the scratch surface has no plane, so its blit zeroes the mask under
its rectangle. With `GpuVoxels` off, a picture read from the voxel cache carries no mask either, so a cached software
vehicle has no highlights; the GPU draws every part afresh, so a vehicle with no turret glows on every pass.

A mask value of exactly 1 written by some other effect over a part's rectangle is taken for the mark and replaced by the
voxel's strength. The value is so faint, and the case so rare, that we accept it; `gpucheck` showed no such pixel.

At a `GpuVoxelScale` above 1 the mask stays at map size: each pixel takes the strength at the centre of its block of
finer pixels, so the glow and the HDR picture follow the sharpened part to within a map pixel.

The check runs under `gpucheck` with the emissive mask on (`EmissiveDebugView=yes`, with `GpuRender=no`) at scale 1 and logs two more lines:

`GPU_VOXEL_MASK frame F part TYPE@FACING pixels P masked M differing D maxdiff X` for each direct part, and
`GPU_MASK frame F pixels P masked M differing D maxdiff X` for each pass, the finished mask against the software plane.
`masked` counts pixels nonzero on either side.

| Scene | Direct parts | Passes | Masked pixels (parts / finished) | Differing |
|---|---|---|---|---|
| `gpu-voxel-compare.txt` (turning vehicles, Kirov, Orca) | 250 | 84 | 107,835 / 89,766 | 0 |
| `hdr-emissive-scene.txt` with `gpucheck` and voxel vehicles added (lasers, bolts, explosions, 1,800 frames) | 16,827 | 577 | 4,057,653 / 2,251,553 | 0 |

The composites in those runs are compared through `GPU_MASK`: their rectangles are zero on both sides.
`tests/autotest/gpu-voxel-bloom.txt` saves a shot of one scene under `Bloom`, for comparing `GpuVoxels` off, on, and on with `GpuVoxelSmooth` and `GpuVoxelScale=4` by eye.

## Other voxel draws

Each shadow, voxel animation and bullet is one part with a `GPU_VOXEL` line of its own, labelled `TYPE@FACINGs` for a
shadow, `TYPE@0a` for a voxel animation and `TYPE@0b` for a bullet, and the finished pass has its usual `GPU_DIFF` line.
At scale 1 every shadow, animation and bullet part compared had 0 differing palette indices. Translucent and predator
voxel draws finish in the hardware blend, so the `GPU_DIFF` of a pass that shows one has the rounding differences
[GPU_MAP.md](GPU_MAP.md) lists under Limits, one step in a 565 channel on the translucent pixels.

| What | Script | Parts compared | Covered pixels | Differing indices | `GPU_DIFF` per pass, mean / most differing, largest step |
|---|---|---|---|---|---|
| Shadows of ten vehicle and aircraft types at 16 facings | `gpu-voxel-compare.txt` | 780 shadows of 1,630 parts | 1,893,968 | 0 | 0 / 0, 0 |
| Cloaked (predator) tank, with the cloak test mod | `cloak-tank.txt` | 3,818 | 2,456,144 | 0 | 161 / 1,905, 2 |
| Warp fade (translucent 50%) on a chronosphere group | `chrono-warp-colour.txt` | 1,344 | 782,651 | 0 | 498 / 1,968, 1 |
| Voxel debris and their shadows, on a mod that gives the Rhino voxel debris | `voxelanim-start-sound.txt` | 668 | 364,308 | 0 | 183 / 1,935, 2 |
| Voxel shells, on a mod that gives the Grizzly shell a voxel | `scalable-bullets.txt` | 4,321 | 2,766,463 | 0 | 749 / 6,177, 3 |
| V3 rockets, Dreadnought missiles (drawn as aircraft) | `v3-attack-then-wait.txt`, `dreadnought-missile-dive.txt` | 1,041 | 1,232,026 | 0 | 147 / 2,589, 2 |

The compared parts come from the voxel image, so they count the voxel drawing alone; the `GPU_DIFF` column includes everything else in the pass, and the largest values come from those passes' translucent blits.

While `GpuVoxels` is on, a shadow is blitted with the depth gradient of the cached blit it replaces (`TechnoClass::Get_Z_Gradient`). The ground gradient of the uncached software path gives exactly the ground's depth, so on flat ground every pixel fails the depth test. A type with `DisableShadowCache=yes` keeps the ground gradient, as in software. A shadow writes no highlight strength.

`GPU_VOXEL` compares the voxel image and `gpucheck` compares against a software replay, so neither shows a shadow the depth test rejects in both. The `gpushadowcount` step reads back the picture a `GpuRender=yes` pass drew and logs one line:

`GPU_VOXEL_SHADOW shadows B pixels P darkened D`

`B` is the voxel shadow blits in the pass, `P` the pixels of their rectangles, and `D` those that show half the brightness of the ground under them. `tests/autotest/gpu-voxel-shadow.txt` runs it at scale 1 and `gpu-voxel-shadow-bloom.txt` at scale 2 under `Bloom`, each with two tanks and a Kirov over sand:

| Script | Shadows | Pixels | Darkened | Darkened with the ground gradient |
|---|---|---|---|---|
| `gpu-voxel-shadow.txt` | 3 | 14,331 | 2,098 | 23 |
| `gpu-voxel-shadow-bloom.txt` | 3 | 14,331 | 1,970 | 25 |

## Checking smooth facing

`tests/autotest/gpu-voxel-smooth.txt` turns two harvesters half a turn, and a tank whose hull and turret turn at their own rates, under `gpucheck` with
`GpuVoxelSmooth=yes`. Each `GPU_VOXEL` line
labels its part `TYPE@F` with `F` the facing in 32nds of a turn: whole numbers with the key off, fractions such as 8.62 with it on,
rising in even steps with no repeats. A composite is labelled `TYPE@FtT`, `T` being the turret's facing in the same units. The software and the GPU draw the same smooth matrix, so the compare stays at 0 differing pixels.

## Cost

With the key on, no voxel object uses the picture cache, so the GPU draws every object from its voxels in every
pass. On `tests/autotest/bench-large-battle.txt` at 1920 by 1080 with `GpuRender=yes` (two runs of 144 seconds
each, `scripts/perf_bench.ps1`), the mean draw took 5.5 ms with `GpuVoxels=no` and 7.8 ms with `yes`, and the
GPU's part of the present 6.0 and 8.6 ms. The CPU's preparation of the voxel objects rose from 0.011 to 0.082 ms
a draw. The run drew 144 times a second with the key off and 84 with it on, at 49.4 and 47.7 game frames a second,
with 1,100 cached voxel pictures a second reused with it off and 1,806 uncached ones drawn with it on. Stage 7
takes this cost down.

## Stages

Each stage has a check that fails when its picture departs from the software.

| Stage | What it adds | Check | Estimate |
|---|---|---|---|
| 1 (done) | Walk, model store, voxel image, shader, direct blits, fallback, `GPU_VOXEL` check | `gpuvoxel` unit test, `gpu-voxel-compare.txt`, zero differing pixels | |
| 2 (done) | Turreted vehicles: hull, turret and barrel drawn into one tile in the order the scratch surface composes them, and the tile blitted like a direct part | `GPU_VOXEL` and `GPU_DIFF` over turreted units at body and turret facings that differ, zero differing pixels | |
| 3 (done) | Emissive mask: write the highlight strength beside the index and feed the glow plane and the HDR composite from it, so `GpuVoxels` works under `Bloom` and HDR Enhanced | `GPU_VOXEL_MASK` and `GPU_MASK` against the software mask at scale 1, zero differing pixels | |
| 4 (done) | The other voxel draws: shadows (the same walk of occupied columns, drawn darkened) and with the smooth key their exact facing, voxel animations, bullets, debris and the translucent, predator and warp flags | `GPU_VOXEL` over shadows at the 16 facings of stage 1, `GPU_DIFF` on the voxel anim and cloak scripts, `gpuvoxel` unit test over the shadow walk | |
| 5 (done) | Smooth facing: build the object matrix from the exact facing instead of the 32-step key and draw every frame without the cache | the angle at each of the 32 keys equals the stage 1 angle (`shapefacing` test); `gpu-voxel-compare.txt` stays at 0 differing pixels; `gpu-voxel-smooth.txt` logs a turning harvester's and tank's hull and turret facings in fractions of a step | |
| 6 (done) | Sharpness: draw at a scale factor, each voxel as the silhouette of its cube instead of a one or two pixel quad, with lighting kept in palette rows (or in float behind a second key) | the scale 1 mode stays index-exact; larger scales are compared by eye and by edge counts | 2 sessions |
| 7 | Settings page switches (greyed unless GPU drawing is on), default on once stage 2 to 5 match, instance buffers and batching, performance numbers on the large battle | the regress suite with the key on | 1 session |

## Where pixel-exact drawing ends

- Stage 5 and 6 change the picture on purpose, so they can only be exact at the facing keys and at scale 1.
- The software writes a voxel's second pixel on the next buffer row when the first is in the last column; the GPU
  clips it. Objects sit near the buffer's centre, so no object we have drawn reaches that column.
- Hardware blending cannot reproduce the software's flooring of halved and quartered terms; this limits the
  translucent blits ([GPU_MAP.md](GPU_MAP.md), Limits), translucent voxels included.
- With `GpuVoxels=yes`, a cloaked voxel object has no ripple: it is drawn as the plain 25% or 50% translucent blend.
  Only the cached voxel images go through `RLE_Blit`, which applies the predator offset, and `GpuVoxels` draws none of
  them. `Bit_Blit` ignores the offset, so an uncached software draw looks the same as the GPU's.
- The model texture holds about a million voxels' worth of walks (a layer is stored once for each of the eight
  orientations it is drawn in) and is emptied when full, so a pass with more distinct layers than that falls back
  to software. The voxel image holds the parts of one pass, several hundred of average size; a pass with more is drawn in software.
- A float lighting model (stage 6) cannot give the software's palette rows, so it is a separate look.
