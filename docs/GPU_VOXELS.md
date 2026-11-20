# GPU voxel drawing

Vehicles, aircraft and other voxel objects are drawn by the software voxel renderer, which projects each voxel
model at one of 32 prepared facings and caches the picture. With `GpuVoxels=yes` the GPU draws them from their
voxels, so that objects can turn smoothly and stay sharp at high resolution. [GPU_MAP.md](GPU_MAP.md) describes the
GPU map pass this builds on, and [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md) how the GPU draws a voxel object into
that pass, with its stages, measurements and limits.

This page owns which objects the GPU draws, voxel placement, stacked shadows, how the software draws a voxel object,
smooth facing, the shadow count, and the record of the image route, the first GPU voxel renderer, which is deleted.

## Where we are

With `GpuVoxels=yes` in `[Video]` of `RA2MD.INI` (default `no`), a voxel object that is blitted straight into the map
view, or a vehicle composed from its hull, turret and barrel, is drawn on the GPU from its voxels instead of from the
software's 256 by 256 picture. The key does nothing unless the GPU draws the pass (`GpuRender`) or the `gpucheck`
autotest step runs. The **Draw vehicles and aircraft in 3D** switch, in the **Vehicles and aircraft** group under Advanced on the Graphics page, sets it.

These objects are drawn on the GPU:

- vehicles and aircraft with no turret (hull only), such as an MCV, a Kirov or an Orca;
- vehicles with a voxel turret and barrel, such as a Grizzly, a Rhino or an Apocalypse, with the turret at any
  facing and the barrel at any pitch;
- the shadows of those vehicles and of aircraft;
- voxel animations and their shadows, which includes voxel debris;
- voxel bullets, such as a V3 rocket or a Dreadnought missile.

The translucent and warp flags of a voxel object are drawn through the same translucent blends as any other blit, and
a predator draw through the plain translucent blend (see "Where pixel-exact drawing ends"). A turreted vehicle whose
body is a shape stays in software (the scratch surface holds a sprite the GPU does not draw).

A voxel object drawn this way never uses the voxel picture cache.

The other keys need `GpuVoxels=yes`:

| Key in `[Video]` | Default | What it does |
|---|---|---|
| `GpuVoxelScale` (0 to 4) | 0 | The scale the map pass is drawn at; see Sharpness |
| `GpuVoxelSmooth` | `no` | Objects turn to their exact facing; see Smooth facing |
| `GpuVoxelLayerDepth` (`Shared`, `Ordered`) | `Shared` | Whether layers and turret pieces hide each other by depth or by the software's draw order ([GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md)) |
| `GpuVoxelCloak` (`Translucent`, `Ripple`) | `Translucent` | Whether a cloaked voxel object ripples ([GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md), "Cloak ripple") |

While the emissive mask is on (`HDRHighlights=Enhanced` with an HDR display, or `EmissiveDebugView`), the GPU also
draws each voxel's highlight strength into the mask that feeds the HDR picture, turreted vehicles included
([GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md), "Stage 5 (done)").

## Smooth facing

With `GpuVoxelSmooth=yes`, an object the GPU draws turns to its exact facing: `GpuDraw_Facing_Radian`
(`code/gpudraw.cpp`) gives `DirType::As_Radian_Smooth` instead of `As_Radian32`, which is the same number at each of the
32 facings. `LocomotionClass::Build_Draw_Matrix` uses it for the hull, and `UnitClass::Unit_Draw_Voxel` for the turret,
turned by the exact turret facing minus the exact hull facing so that the turret lands at its own facing on top of the
hull's. The barrel pitch stays in 32 steps. `FacingClass::Current` already reads the facing part way between two game
frames when `RenderFrameRate` is above the game rate (`RenderBlend`), taking the short way round, so the smooth angle
follows it for the hull and for the turret alike. A shadow turns the same way: `LocomotionClass::Build_Shadow_Matrix`
and `FlyLocomotionClass::Build_Shadow_Matrix` use `GpuDraw_Facing_Radian` too, so the shadow stays under its object.
With the key off, nothing changes.

`tests/autotest/gpu-voxel-smooth.txt` turns two harvesters half a turn, and a tank whose hull and turret turn at their
own rates, under `gpucheck` with `GpuVoxelSmooth=yes`. It checks that the game draws those passes without a crash; its
`GPU_DIFF` numbers vary with when a pass is read back, so regress compares only the crash lines.

## Voxel placement

`SteadyVoxelPlacement` (`[Video]`, default `yes`) projects each voxel scene about a whole-pixel centre. It applies to the software
drawing and to `GpuVoxels` alike. `VoxelDrawSystem::Scene_Center` (`code/voxdrsys.h`) gives the point, and both `Draw_Prepared` (software)
and `Capture` (GPU) read it.

The original game projects the model about the exact fractional centre of the scene, but `Region_Of` places the picture at the centre with its
fraction cut off. A turning vehicle's centre moves by fractions of a pixel from facing to facing, so its picture sits up to a pixel off its model
and wobbles as it turns. With the key on, the centre is rounded first, and the cut removes nothing. At 4K and zoom 2 with units turning, the vertical hops of a Grizzly and an
Apocalypse went from 16 and 2 pixels to 0 and 0, and the horizontal ones from 35 and 18 to 7 and 9.

Classification: intentionally changed. The default departs from gamemd, and `SteadyVoxelPlacement=no` keeps gamemd's placement. Pictures differ by up to a pixel in
position, so the shadow counts and pictures that tests record change with the default. Only drawing changes, and the simulation is the same.

Changing the key while the game runs clears every voxel picture cache at the start of the next frame (`ObjectTypeClass::Clear_Voxel_Indexes`).
That also invalidates the turreted-vehicle pictures.

## Stacked shadows

`ShadowStacking` (`[Video]`, default `yes`) decides what overlapping voxel shadows do to the ground. With `yes` each shadow darkens the ground again, as in
gamemd: `TechnoClass::DrawVoxelShadow` draws every shadow through a darken blit with no shared mask, so eight Kirovs over one cell leave the ground
near black. With `no` the ground under overlapping shadows darkens once, with hard edges. The key applies to software drawing and to every
`GpuRender` path, and a change takes effect on the next frame. Only drawing changes: `pacing-determinism-fast.txt` gives the same 1,500 state hashes with both values.

- **Software.** The three voxel shadow blits (the cached shadow, the uncached one, and the shadow of a voxel animation) run through `ShadowOnceRows` (`code/shadowonce.h`). It keeps a mark per pixel of the
  surface holding the frame number of the shadow that last darkened the pixel, and puts back a pixel that a mark from this frame covers. `Tactical::Render_Foreground` starts a frame.
  A pixel an opaque object has since covered keeps its mark, so a later shadow does not darken that object.
- **GPU.** Every shadow in the pass goes into the shadow mask, and the ground is darkened once through it, with `GpuVoxels` off and on. The
  mask holds every darkening blit of the pass, so the shadows of shapes drawn in the pass (infantry, buildings) are unioned too, which software does not do.
- Shadows that buildings and trees draw into the tile layer are not changed by the key.

Classification: preserved with the default, and intentionally changed for `no`. Eight Kirovs over one cell, window brightness over a 32 by 10 patch of shadowed sand (`shadow-stack-*.txt`, `windowdark`), as a fraction of the unshadowed ground:

| Kirovs | 1 | 2 | 4 | 8 |
|---|---|---|---|---|
| `yes` | 0.49 | 0.24 | 0.05 | 0.01 |
| `no`, software | 0.49 | 0.49 | 0.49 | 0.49 |
| `no`, GPU | 0.50 | 0.50 | 0.50 | 0.50 |

The GPU values match the default's at one Kirov and are slightly lighter than software's because the mask darkens the ground picture before objects are drawn.
With eight Kirovs `no` costs software drawing about 0.09 ms a frame at 800 by 600.

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
6. **Caches.** A picture is cached per type and facing key (`VoxelIndexClass`, `VoxelStaticBuffer`), and a turreted
   vehicle's composed picture is cached too. With `GpuVoxels=no` the GPU map pass keeps those cached pictures in its
   atlas ([GPU_MAP.md](GPU_MAP.md)). A shadow is drawn separately by `Render_Shadow`: the occupied columns of the
   layer, flattened onto the ground and slid along the light vector, blitted darkened.

## The GPU path

A part is one prepared object: its layers' `VoxelRenderStruct` copies, the projection centre, the lighting table and
the part of the voxel buffer it covers (`VoxelDrawSystem::Capture`, `GpuVoxelPart` in `code/gpudraw.h`). The blit that
shows a part is recorded without pixels, and the map pass draws the part's layers in the blit's place.

- `Gpu_Voxel_Walk` (`code/gpuvoxel.cpp`) lists a layer's voxels in the order the drawer writes them, with their
  place in the walk, colour and normal. `tests/gpuvoxel` draws that list on the CPU the way the shader does and
  compares it with the software drawers for random layers in all eight orientations.
- `code/gpumapvoxel.cpp` keeps those lists in a 2048 by 8192 model texture, two texels a voxel, one list for each
  layer and orientation, each right after the one before.
- A shadow is one flat layer. `Gpu_Voxel_Walk_Shadow` lists one voxel with K 0 and colour 1 for each occupied column,
  and `VoxelLibrary::Setup_Shadow` gives the projection the software shadow drawer uses. A voxel animation's shadow is
  drawn the same way.
- A turreted vehicle is one part with a piece for each of its hull, turret and barrel. The software composes them on
  the 160 by 160 scratch surface, where a later piece replaces the pixels below it that it draws, and the scratch
  surface passes each pixel through the colour translation of its own drawer. The GPU places each piece where it
  would land on the scratch surface and reads the palette texture's second block, which is the first run through that
  translation.
- A prepared object is kept under a key made of its voxel library, motion library and frame, its matrix, the camera,
  the light and the shading rules, and a shadow likewise. An object drawn again with an equal key takes the kept
  preparation. Any new input to the preparation or the lighting table must be added to that key (`GpuVoxelPrepKey` in
  `code/gpudraw.h`).
- While the GPU draws a pass, the software renders none of a part's pieces and writes nothing to the scratch surface.
- A pass the GPU cannot draw (a limit, a failed program) renders the parts in software when it falls back, so the
  picture is complete.

## Sharpness

`GpuVoxelScale=0`, the default, takes the scale from the zoom: the screen pixels one map pixel covers (the map view's
zoom times the interface scale), rounded to the nearest whole number from 1 to 4, so a view shown at less than 1.5
screen pixels a map pixel draws at scale 1. `Gpu_Voxel_Pass_Scale` (`code/gpuvoxel.cpp`) computes it for each picture.

At a scale above 1, `GpuRender` draws the whole map pass at that many times the map's own pixels. The scene
and snapshot textures are scale times the area in each direction, and the pass's views are scaled the same way, so
every blit keeps its map pixel coordinates and the shader shows each map pixel as a block of scale by scale pixels.
The shadow mask and the emissive planes stay at map size. `bgfxbackend.cpp` treats the texture as scale times larger, so the presenter smooths it when it is
shown smaller than that and keeps whole pixels when it is shown at that size or larger. Zooming in the map view (`viewzoom.cpp`)
therefore shows the finer picture.

At scale 1 a voxel is the line of one or two pixels the software writes. Above it a voxel is drawn as the exposed
faces of its cube with `GpuVoxelLayerDepth=Shared`, and as the outline of its cube with `Ordered` or when the faces of
a pass do not fit the face store. Every voxel object keeps the scale of its pass.

A pass that `gpucheck` reads back is drawn at scale 1, because the software result it is compared with has no finer
pixels. `tests/autotest/gpu-voxel-scale.txt` shows two tanks and a Kirov at three zooms for comparing scales by eye
and by edge counts.

A shadow's column is drawn as its flat cell, so a shadow stays in register with its object at any scale. With
`ShadowStacking=no`, a shadow's layers are drawn into the map-sized mask at scale 1.

## Voxel shadows on the GPU

While `GpuVoxels` is on, a shadow is blitted with the depth gradient of the cached blit it replaces (`TechnoClass::Get_Z_Gradient`). The ground gradient of the uncached software path gives exactly the ground's depth, so on flat ground every pixel fails the depth test. A type with `DisableShadowCache=yes` keeps the ground gradient, as in software. A shadow writes no highlight strength.

`gpucheck` compares against a software replay, so it does not show a shadow the depth test rejects in both. The `gpushadowcount` step reads back the picture a `GpuRender=yes` pass drew and logs one line:

`GPU_VOXEL_SHADOW shadows B pixels P darkened D twice T`

`B` is the voxel shadow blits in the pass, `P` the pixels of their rectangles, and `D` those that show half the brightness of the ground under them. `T` counts the pixels inside only one shadow's rectangle that show the ground darkened two or three times, to within one 8-bit step of what the darken blend gives, which a shadow drawn as overlapping cells without a once-per-blit rule leaves; it is 0 in these scenes, and about 500 at scale 1 when hard shadows are drawn without their stencil. A part of an object whose colour matches such a value in all three channels would also count. Two scripts run it with two tanks and a Kirov over sand:

| Script | Scale | Shadows | Pixels | Darkened |
|---|---|---|---|---|
| `gpu-voxel-direct-shadow.txt` | 1 | 3 | 14,331 | 2,239 |
| `gpu-voxel-shadow-mask.txt`, with the emissive mask on | 2 | 3 | 14,331 | 2,074 |

With the ground gradient in place of the cached blit's, the first two scenes darkened 23 and 25 pixels, measured on the image route before a placement fix moved the two tanks over their shadows.

`gpu-voxel-overflow.txt` runs the count over 300 tanks, more shadow blits than the pass has stencil values ([GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md), "Stage 8 (done)").

## Where pixel-exact drawing ends

- Smooth facing and a scale above 1 change the picture on purpose, so the GPU can only match the software at the 32
  facings and at scale 1, and there only with `GpuVoxelLayerDepth=Ordered`.
- Hardware blending cannot reproduce the software's flooring of halved and quartered terms; this limits the
  translucent blits ([GPU_MAP.md](GPU_MAP.md), Limits), translucent voxels included.
- A cloaked voxel object is drawn as the plain 25% or 50% translucent blend, by the software and by the GPU.
  `RLE_Blit` and `Bit_Blit` both give their blitter a warp offset of 0, so the `Warp` blitters read the pixel they write.
  `GpuVoxelCloak=Ripple` adds a ripple the software does not draw ([GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md), "Cloak ripple").
- The model texture holds 8,388,608 voxels' worth of walks (a layer is stored once for each of the eight
  orientations it is drawn in) and is emptied when full. A pass of a large Mental Omega mission needed 2.4 million,
  which the first size of 2,097,152 did not hold. [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md) lists what
  happens to a pass that does not fit it, under "Fallback still switches look".

## The image route (deleted)

The first GPU voxel renderer drew each part into a tile of a 2048 by 2048 voxel image of palette indices, and the
part's blit read its tile through the map shader like any other blit. It was built in seven stages (walk, model store
and voxel image; turreted vehicles; highlights; shadows, animations and bullets; smooth facing; sharpness; kept
pictures), and stage 8 of [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md) deleted it once the GPU drew every part
straight into the pass. The walk, the model texture, the prepared objects, smooth facing and the shadow count above
come from it. The numbers below are what it measured, kept as the record the direct route was checked against.

**How it worked.** `vs_mapvoxel.sc` drew each voxel as a line of one or two pixels into the part's tile, in the
software's write order, so later voxels overwrote earlier ones. A tile was the part's buffer region times
`GpuVoxelScale`. A part had a signature, a 64-bit hash of everything the shader's output depended on, and a part
whose signature the image already held was not drawn again. When the parts of a pass did not fit an empty image at
the chosen scale, the largest parts were drawn at a lower scale, and a pass whose parts did not fit at scale 1 was
drawn in software. While the emissive mask was on, the tile's green channel held each voxel's highlight strength, and
the mask pass copied it wherever a part's blit had left a mark in the CPU plane.

**Exactness.** Under `gpucheck` every part drawn in a checked pass was read back from its tile, and a `GPU_VOXEL`
line compared its palette indices with the software's. `tests/autotest/gpu-voxel-compare.txt` plays eight vehicle
types turning through sixteen facings, with a Kirov and an Orca overhead, then turns the turrets of five turreted
types to facings other than the body's:

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

Sixty-five of the 567 composites had a turret facing different from the body's. The `GPU_DIFF` lines of the same run
had the same one-step pixels, about three a pass, as the run with `GpuVoxels=no`; none of them lay on a vehicle.

The other voxel draws were compared the same way. Translucent and predator draws finish in the hardware blend, so
the `GPU_DIFF` of a pass that shows one has the rounding differences [GPU_MAP.md](GPU_MAP.md) lists under Limits.

| What | Script | Parts compared | Covered pixels | Differing indices | `GPU_DIFF` per pass, mean / most differing, largest step |
|---|---|---|---|---|---|
| Shadows of ten vehicle and aircraft types at 16 facings | `gpu-voxel-compare.txt` | 780 shadows of 1,630 parts | 1,893,968 | 0 | 0 / 0, 0 |
| Cloaked (predator) tank, with the cloak test mod | `cloak-tank.txt` | 3,818 | 2,456,144 | 0 | 161 / 1,905, 2 |
| Warp fade (translucent 50%) on a chronosphere group | `chrono-warp-colour.txt` | 1,344 | 782,651 | 0 | 498 / 1,968, 1 |
| Voxel debris and their shadows, on a mod that gives the Rhino voxel debris | `voxelanim-start-sound.txt` | 668 | 364,308 | 0 | 183 / 1,935, 2 |
| Voxel shells, on a mod that gives the Grizzly shell a voxel | `scalable-bullets.txt` | 4,321 | 2,766,463 | 0 | 749 / 6,177, 3 |
| V3 rockets, Dreadnought missiles (drawn as aircraft) | `v3-attack-then-wait.txt`, `dreadnought-missile-dive.txt` | 1,041 | 1,232,026 | 0 | 147 / 2,589, 2 |

**Highlights.** With the emissive mask on, `GPU_VOXEL_MASK` lines compared each part's strengths and `GPU_MASK` lines
the finished mask of each pass with the software plane, at scale 1:

| Scene | Parts | Passes | Masked pixels (parts / finished) | Differing |
|---|---|---|---|---|
| `gpu-voxel-compare.txt` (turning vehicles, Kirov, Orca) | 250 | 84 | 107,835 / 89,766 | 0 |
| `hdr-emissive-scene.txt` with `gpucheck` and voxel vehicles added (lasers, bolts, explosions, 1,800 frames) | 16,827 | 577 | 4,057,653 / 2,251,553 | 0 |

A turreted composite wrote no mask on the image route, as in the software.

**Cost.** `tests/autotest/bench-large-battle.txt` and `bench-late-game.txt` at 3840 by 2160 with `GpuRender=yes`,
`RenderFrameRate=240` and HDR Enhanced highlights, before and after the image route kept prepared objects and tile
pictures between passes. Each cell pools two runs, and the runs of the old and the new build alternated on a machine
in other use, so differences of a few tenths of a millisecond are noise. "On" is `GpuVoxels=yes` with
`GpuVoxelSmooth=yes` at the given `GpuVoxelScale`.

| Benchmark | Keys | Mean draw before | after | p95 before | after | Worst before | after | Draws a second before | after |
|---|---|---:|---:|---:|---:|---:|---:|---:|---:|
| Large battle | off | 0.86 | 0.88 | 3.48 | 2.53 | 28.1 | 15.7 | 297 | 297 |
| Large battle | on, scale 1 | 3.54 | 2.18 | 6.01 | 4.83 | 18.2 | 19.3 | 208 | 237 |
| Large battle | on, scale 2 | 3.56 | 1.91 | 8.30 | 3.94 | 19.5 | 15.7 | 207 | 240 |
| Late game | off | 2.75 | 2.55 | 12.33 | 11.65 | 21.8 | 22.8 | 152 | 157 |
| Late game | on, scale 1 | 5.07 | 4.69 | 15.01 | 14.08 | 24.0 | 29.4 | 111 | 113 |
| Late game | on, scale 2 | 4.48 | 3.89 | 13.33 | 12.78 | 21.6 | 22.2 | 123 | 132 |

On the large battle at scale 1 a pass had about 110 parts, 196 layer draws and 531,000 voxels. `draw.voxels` fell
from 0.82 to 0.17 ms a draw, and `present.gputime` from 1.11 to 0.76 ms at scale 1 and from 4.29 to 1.26 ms at scale
2. Of the parts in a second, 5% had to be drawn at scale 1 and 28% at scale 2; on the late game 24% and 37%. With the
key on, `draw.gpumap` stayed about 0.4 ms above its value with the key off on the large battle and 0.4 to 0.7 ms above
it on the late game. [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md) takes these numbers as its baseline.
