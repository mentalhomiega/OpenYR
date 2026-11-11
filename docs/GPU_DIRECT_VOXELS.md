# Drawing voxels straight into the GPU scene (design)

We propose to draw each voxel object as geometry into the map pass's scene target, at the place of its blit in the
pass, with a depth and stencil buffer for the voxels' own depth. The voxel image, its tiles and its overflow go away,
sharpness becomes the resolution of the scene target, and hull, turret and barrel hide each other by real depth.
Stages 1 to 5 are built, and `GpuVoxelPath=Direct` is the default; `GpuVoxelPath=Image` keeps the image route; the "(done)" sections after the Stages table say what
they delivered and measured. Every "we propose" elsewhere is a design choice for the
later stages, and every statement about current behaviour cites the code. [GPU_VOXELS.md](GPU_VOXELS.md) owns the
image route and [GPU_MAP.md](GPU_MAP.md) the map pass.

The GPU path no longer has to match gamemd pixel for pixel. The software renderer stays the reference and the classic
look (`GpuRender=no`, or `GpuVoxels=no`); the GPU path becomes the modern look. Stage 1 still has an exact mode, so
its first check can demand zero differing pixels.

## Where we start

The object pass is recorded as a list of blits and drawn in that order over the uploaded tile layer.

| Part | Code | What it does today |
|---|---|---|
| Recording | `code/gpudraw.cpp`, `GpuBlit` and `GpuVoxelPart` in `code/gpudraw.h` | Each blit keeps its rectangle, translator, lighting remap, blend kind and row depth (`ZFirst`, `ZFraction`, `ZNumerator`, `ZDenominator`, `ZWrap`). A voxel object's blit has no pixels; `Voxel`, `VoxelX` and `VoxelY` name its part and where the blit starts in it. |
| Depth | `Upload_Pass_Stores` in `code/gpumap.cpp`, `fs_mapdraw.sc` | The software depth buffer goes up as an R16 texture (`s_zbuf`). A blit fragment is discarded unless its row depth minus its depth shape is below the buffer value. Nothing in the pass writes depth: `BlitDesc` has `ZRead` and no write flag (`code/blitter.h`), and a blitter that cannot describe itself counts as `Unsupported` (`Record_RLE`, `Record_Bit`). The scene framebuffer has no depth attachment (`Size_Target`). |
| Order | `Submit_Draws`, `Set_Map_View` | One draw per blit, in recorded order, in `ViewMode::Sequential` views. Objects cover each other by this order alone. |
| Voxel parts | `GpuVoxel_Submit` in `code/gpumapvoxel.cpp`, `vs_mapvoxel.sc` | Each part not already in the 2048 by 2048 RG8 voxel image gets a tile of its buffer region times its tile's scale, which is `GpuVoxelScale` or, when the pass would not fit, a lower one. Its layers are drawn into the tile in the software's walk order: a 2-vertex line a voxel at scale 1, an 8-vertex outline strip above. |
| Voxel blits | `Source_Offset`, `fs_mapdraw.sc` | A negative offset, -1 minus a texel of the voxel image, reads the tile, then runs the translator, alpha lighting, row depth test and blend as for any blit. |
| Model store | `Load_Models`, `Model_Key` | A layer's walk is stored once for each of the eight anchor orientations it is drawn in. |
| Highlights | `Submit_Voxel_Mask`, `fs_mapmask.sc` | A direct part's blit leaves `GPU_VOXEL_MASK_MARK` in the CPU plane; the mask pass replaces the mark with the strength in the part's tile. |

The image route has four limits that this design removes.

- **Overflow.** A tile's side grows with the scale, so its area grows with the scale squared. Before the overflow fix, a
  pass whose tiles did not fit an empty image was drawn in software: with 48 Kirovs at 4K, 595 of 600 passes fell back,
  and the picture switched between the GPU look and the software look. Since the fix (`Place_Parts`, `TILE_CAPS` in
  `code/gpumapvoxel.cpp`), the largest parts of such a pass get tiles at a lower scale, down to 1, and only a pass whose
  parts do not fit at scale 1 is drawn in software; the 48 Kirovs then lose their sharpness instead.
- **Picture upkeep.** Parts are hashed (`Part_Signature`), placed on shelves (`GpuVoxelTileStore`) and drawn into tiles;
  the image is emptied whole when it fills or when the palette texture changes.
- **Depth.** A voxel blit tests one depth per row, from the object's gradient (`TechnoClass::Get_Z_Adjust`,
  `Get_Z_Gradient`, the `ZGradients` table in `code/_zbuffer.cpp`). Hull, turret and barrel are ordered by the order
  `UnitClass::Unit_Draw_Voxel` composes them, which swaps barrel and turret by turret facing.
- **Sharpness.** A tile is the software's buffer region times the scale, so the picture is capped by the tile.

The `voxel-depth-cubes` experiment (`scratch-agent/voxel-cubes/REPORT.md`) drew voxels as depth-tested cubes inside the
tile at scale 2 and got the same picture as the ordered drawing, because for whole cubes a layer's walk is already back
to front. Depth testing cubes therefore costs nothing in look, and it frees the drawing from the walk order.

## Target design

### Where a voxel object is drawn

We propose that `Submit_Draws` keeps its one loop over the blits. For a blit whose part the direct route draws, it
submits the part's layer draws (a new program, `vs_voxelscene.sc` with `fs_voxelscene.sc`) in place of the blit's quad,
in the same view. bgfx keeps submission order in a sequential view, so every sprite, line, effect and other object keeps
its place in the order. The blit stays in the list as the carrier of everything the part needs from the software: its
rectangle (the clip, including a sinking object's waterline), translator, lighting remap, alpha level, kind and row
depth.

The part's place in the scene follows from the blit: the part's buffer region starts at map pixel
(`blit.X - blit.VoxelX`, `blit.Y - blit.VoxelY`). For a composite, each piece lands at `piece.Dest - part.Area` from
there, the same arithmetic `GpuVoxel_Submit` uses for `shift_x` and `shift_y` with the tile origin replaced by that
map position. The draw is scissored to the blit's rectangle times the scale.

### Geometry

We propose two stages of geometry.

- **Stage 1 reuses the current primitives.** The model store, the walk, the 16-bit projection steps from
  `VoxelLibrary::Setup_Object`, and the line or outline strip of `vs_mapvoxel.sc` stay as they are. Only the output
  changes: scene pixels instead of tile pixels, a depth, and a flat palette index and highlight strength for the
  fragment shader. Vertex pulling from the model texture through `gl_VertexID` stays (`BGFX_CAPS_VERTEX_ID`, checked
  in `Start` of `code/gpumapvoxel.cpp`).
- **Stage 6 draws faces.** For each layer we build once, at first sight, the list of its exposed faces: a face whose
  neighbour voxel in that direction is empty. Each face is one 8-byte record (voxel x, y and z, direction, colour,
  normal) in a face store texture, sorted into six ranges by direction. A 2048 by 2048 RG32U texture holds 4,194,304
  faces in 32 MB. Per draw we submit only the three ranges that face the camera, six vertices a face, positions
  pulled by `gl_VertexID`. The store is keyed by the library's serial and the layer, with no orientation, because
  depth testing makes the walk order unnecessary. This replaces the eight orientation copies of the model store
  for drawing; the exact mode, `GpuVoxelLayerDepth=Ordered`, keeps the stage 1 primitives and the walk, since faces
  have no walk order.

We do not propose instancing: each layer is already one draw of pulled vertices, and the per-draw data is a few
uniforms. A per-pass instance texture becomes worth it only if draw calls show up in the measurements.

Below about two screen pixels a voxel, faces turn into pixel-sized triangles, which GPUs shade in 2 by 2 quads at a
large waste (Fatahalian et al., 2010). We therefore keep the one-primitive splat (the stage 1 line) as the level of
detail whenever a voxel's projected step is under a threshold, which is the normal case at `GpuVoxelScale=1`. A
compute-shader splat renderer (Schütz et al., 2021) is the option if the splat itself becomes the cost. A ray-cast
box per voxel (Majercik et al., 2018) gives exact cube edges but writes depth from the fragment shader, which turns
off early depth rejection; we do not plan it.

### Depth

#### Inside an object: depth slices

We propose a D24S8 depth and stencil attachment on the scene framebuffer, cleared at the start of `VIEW_MAPDRAW` and
kept through the sonic wave views, which draw into the same framebuffer. The blits other than direct voxel draws
neither test nor write it, as now.

Each direct blit gets its own depth slice. The n-th direct blit in blit order writes depth
`((SLICES - 1 - n) + d) / SLICES`, where `d` in [0, 1) places the voxel inside the part (see the modes below), and
tests with `LEQUAL`, which gives a tie to the later voxel as the walk does. A later part therefore always passes over
an earlier one, which keeps the painter's order between objects, and inside a part `d` decides. With 24 bits and
4,096 slices, each slice has 4,096 depth steps. A pass with more than 4,096 direct blits sends their parts to the image
route (stage 1); a depth clear, which means a new view, would lift that limit.

Inside a part we offer two modes (proposal, key `GpuVoxelLayerDepth`):

| Mode | What `d` is | Use |
|---|---|---|
| `Ordered` | The drawing order: each layer, or composite piece, gets a sub-slice in the software's order, and inside a layer `d` falls with the voxel's place in the walk | The exact mode. A later voxel always passes, so the scene gets the pixels the tile gets today; the checks of stages 1, 2, 4 and 5 use it |
| `Shared` | The voxel's view depth across the whole part | Default: the nearer voxel wins, so a barrel behind the turret is hidden by depth rather than by a facing-dependent order |

`Ordered` is needed for exactness because the walk is ordered by row, column and position in the column, which is
not always nearest last for two voxels whose one- or two-pixel stamps overlap; the experiment's result holds for whole
cubes. A layer of more than 4,096 voxels shares depth steps between neighbours in the walk, which keeps the order,
because an equal depth passes `LEQUAL`.

The view depth of a voxel comes from the prepared layer. `VoxelRenderStruct::BoxCorner` holds the layer box's corners
after the camera and object matrices (`VoxelDrawSystem::Prep_For_Object`), and `VoxelScene::Center` is the box
centre in the same space, whose x and y place the picture (`Region_Of` in `code/voxdrsys.cpp`). From the anchor
corner and its neighbours along the walk axes we get a depth origin and three depth steps per layer, uploaded beside
the 2D steps. Which corner belongs to which walk axis must be read from `VoxelRenderOrientations` and checked against
`Setup_Object` before stage 1 code is written.

`Shared` needs the pieces of a composite on one depth axis. Each piece is prepared with its own matrix, so this holds
when those matrices carry the turret and barrel offsets; stage 2 must confirm it before it relies on it.

#### Against the map: the 2.5D test

The map's depth is not a view depth. The software computes a blit row's depth from the depth buffer's scroll and the
screen row (`DepthBuffer->Get_Scroll_Delta` in `Bit_Blit`, `code/blit.cpp`), plus the object's adjust; for a raised
object `TechnoClass::Get_Z_Adjust` subtracts its height in pixels, so its rows test at about the depth of the ground
row under it. A fragment passes where that depth minus any depth shape is below the buffer value (`fs_mapdraw.sc`).

We propose that `fs_voxelscene.sc` runs the same test with the blit's own row depth, the row being the fragment's map
row minus `blit.Y`. Rows above or below the blit's rectangle cannot occur, because of the scissor. This keeps every
`ZFudge*` adjust, ramp bias and bridge rule exactly as the software applies them, and it is the stage 1 behaviour
(scene depth mode `Row`).

A per-voxel scene depth (mode `Voxel`) is the later option: the blit's depth at the row of the object's ground point,
plus the voxel's own footprint offset in rows, so the front of a long vehicle can pass in front of a wall its back is
behind. It may also clip vehicles into ramps and bridges whose fudges were tuned for one depth per row, so we propose
it as a mod key (see Settings), default `Row`, and do not plan it before stage 7.

The test discards fragments, so voxel draws lose early depth rejection. Measure it before optimising; the image route
pays the same discard in `fs_mapdraw.sc` today.

### Colour: palette, house colours, lighting

The palette model stays. The vertex shader turns the voxel's normal into a lighting row through the object's table
(`Precalculate_Normal_Lookup`, `code/voxlib.cpp`; a `u_vlight` uniform until stage 7, a row of `s_vpal` since) and the
row and colour into a palette index through `VoxelPaletteTranslateTable` (`s_vpal`, `Upload_Palette`). The fragment shader then does what `fs_mapdraw.sc`
does for a copy blit: adds the alpha lighting band from `s_alut` at the blit's level, reads the 565 colour from the
blit's translator row in `s_lut`, which carries the house colours, and writes it. A composite reads the second block
of `s_vpal`, the scratch surface's translation, as now.

We propose to move the depth test, the translator lookup and `Rgb565` from `fs_mapdraw.sc` into a shared include
(`mapcommon.sh`) that both fragment shaders use, so the two cannot drift.

We planned to compute the lighting row on the GPU in stage 6, from the normal tables and the light vector, so that
`u_vlight` and the CPU half of `GpuDraw_Capture_Prepared` would lose their per-object table. Stage 6 left it out, and
stage 7 moved the table into `s_vpal` instead: the CPU still computes it. The rows stay palette rows. A float lighting
model is a separate look (GPU_VOXELS.md, "Where pixel-exact drawing ends") and stays outside this design. The face
shading of the unmerged `voxel-depth-cubes` branch (`GpuVoxelFaces`) fits stage 6 directly, since each face knows its
direction.

### Translucency, warp and cloak

A voxel blit's kind comes from `converter->Blitter_From_Flags(GpuDraw_Voxel_Blit_Flags(flags))` in
`TechnoClass::Draw_Voxel` (`code/techno.cpp`): a copy, or 25%, 50% or 75% translucent for cloak and warp.
`GpuDraw_Voxel_Blit_Flags` drops `SHAPE_PREDATOR`, so a cloaked object is the plain translucent blend.

Drawn as geometry, a translucent object would blend once for every voxel that covers a pixel. We propose a depth
pre-pass for a translucent part: its layers are drawn once writing depth only, then again with `EQUAL` and the
translucent blend from `Blend_State_For`, so each pixel blends once with the nearest voxel. Only cloaked and warping
objects pay the second draw.

The cloak ripple is an option (`GpuVoxelCloak=Ripple`, built; "Stage 4" below describes it). The software draws no ripple
today: `RLE_Blit` takes the predator offset as an unnamed parameter and gives its blitter a warp offset of 0, and
`Bit_Blit` does the same, so the `Warp` blitters read the pixel they write. The ripple is therefore a new look of the
direct route, not a copy of the software's.

### Shadows

A voxel shadow is one flat layer of occupied columns (`Gpu_Voxel_Walk_Shadow`, `VoxelLibrary::Setup_Shadow`), blitted
with the darken kind, or, under `SoftShadows`, drawn into the shadow mask (`Submit_Shadows`). We keep that geometry: each
column becomes its flat cell, as at scale 2 and up today, drawn straight into the scene or the mask.

Cells of one shadow overlap, and darken must apply once per shadow blit, as the tile gives today. We propose the stencil:
shadow blit n draws with reference `n % 255 + 1`, passes where the stencil differs and replaces it, so each pixel
darkens once per blit, and two different shadows still darken twice, as in the software. Every 255 shadow blits a
stencil-only quad resets the stencil. Under `SoftShadows` the mask's `max` of 1 already makes overlaps harmless.

The row depth of a shadow stays as `Techno_Render_Voxel_Shadow` sets it (the object's gradient while `GpuVoxels` is on).
Real shadow mapping onto the ground needs GPU terrain and is not part of this design.

### Highlights, bloom and HDR

We propose a second scene attachment, RG8 at scene size: highlight strength and the low byte of the part's ordinal.
Direct voxel draws write it with the same strength formula as `vs_mapvoxel.sc`; every other map fragment shader writes
a second output that the blend keeps unchanged (`BGFX_STATE_BLEND_INDEPENDENT`, factors zero and one for the second
target). The mask pass keeps its structure: a copy of the CPU plane, then one quad per direct part in blit order, which
takes the attachment's strength where the plane holds `GPU_VOXEL_MASK_MARK` and the attachment's ordinal is the part's,
and zero elsewhere in the rectangle. This keeps today's rules, including a later sprite over the part (the plane no
longer holds the mark) and a transparent pixel of a later part over an earlier one (ordinal mismatch, zero, as the
software's mask copy gives). At scale above 1 the mask takes the centre of each block, as now.

If writing a second output from every map shader proves awkward, the alternative is a second draw of the direct
geometry with `EQUAL` depth into a separate target, paid only while the mask is on.

The glow (`Submit_Glow`), the post pass and the HDR composite ([EMISSIVE_MASK.md](EMISSIVE_MASK.md)) read the finished
mask and do not change. A composite writes no mask on the image route because the scratch surface has none; a direct
composite writes one, so turreted vehicles glow too. That is a deliberate change from the software.

Stage 5 built the alternative above in place of the RG8 attachment: a second draw of the direct geometry, into the
mask target, with that target's own depth attachment; "Stage 5 (done)" describes it.

### Facing and tilt

The direct route draws whatever matrix it is given, so smooth facing needs no new code: `GpuDraw_Facing_Radian`
already gives `LocomotionClass::Build_Draw_Matrix` and `UnitClass::Unit_Draw_Voxel` the exact facing with
`GpuVoxelSmooth=yes`, and the barrel pitch stays in 32 steps.

Tilt on a slope comes from `Get_Slope_Matrix` for the cell's ramp (`DriveLocomotionClass::Build_Draw_Matrix`,
`code/drive.cpp`), so it changes at once at a cell edge. A smooth tilt (proposal, later, `GpuVoxelSmoothTilt`) would
blend from the previous slope matrix by the object's progress across the cell, in the draw code only. Every blended
matrix is a new prepared-object key (`GpuVoxelPrepKey`), so it raises preparation cost until stage 6 moves lighting
to the GPU.

### Sharpness

Sharpness becomes the scene target's resolution. `GpuVoxelScale` already draws the whole pass at that many times the
map's pixels (`Size_Target`, `Set_Map_View`), and the direct geometry draws at whatever resolution the target has, with
no tile in between. `GpuVoxelScale=0` (stage 7) takes the scale from the zoom: the screen pixels one map pixel covers,
rounded to the nearest whole number from 1 to 4, which `Pass_Scale` then lowers to the texture limits below.

## What changes for the player

| Look or cost | Classic (software, or `GpuVoxels=no`) | Image route today | Direct route |
|---|---|---|---|
| Picture at scale 1 | gamemd's | Index-exact with the software | Equal with `GpuVoxelLayerDepth=Ordered`; with `Shared`, layers and turret pieces hide each other by depth |
| Large scale with many large units | Unchanged | Draws the largest parts at a lower scale, down to 1 | No voxel image to overflow, so every part keeps the scale; the remaining limit is the face store, about 4 million faces at the proposed size |
| Hull, turret and barrel | Order by turret facing | Same | Depth decides |
| Cloaked unit | Plain translucent (the predator offset is not applied) | Plain translucent | Plain translucent, or a ripple with `GpuVoxelCloak=Ripple` |
| Turreted vehicle under Glow or HDR Enhanced | No highlight | No highlight | Highlights |
| Turning and tilt | 32 facings | Exact facing with `GpuVoxelSmooth` | Same; smooth tilt as a later option |
| Main thread | Software voxel drawing | Tile hashing, shelf placement, part signatures | No tiles or signatures; per-layer uniforms only |
| GPU | None | Draws only parts not in the image (5% to 37% of parts a second in the Cost runs of GPU_VOXELS.md) | Draws every visible voxel every pass; we expect it to cost more at scale 1 and less where tiles were redrawn often |

The classic look keeps what the software draws and stays the reference
for `gpucheck` and `GPU_VOXEL`.

## Stages

Each stage has a check that fails when its result breaks. Until stage 8 a part the current stage does not handle takes
the image route in the same pass, so no stage adds a software fallback.

| Stage | What it adds | Check | Shares | Deletes |
|---|---|---|---|---|
| 1 (done) | Direct opaque parts drawn into the scene: vehicles without a turret, aircraft, voxel animations, debris, bullets. D24S8 attachment, depth slices with `Ordered` and `Shared` layer modes, row depth test from the blit, translator and lighting band in `fs_voxelscene.sc`, `GpuVoxelPath=Direct`. While the emissive mask is on, these parts stay on the image route. Built with the hard shadows of stage 3, which the 48-Kirov check needs | `gpu-voxel-compare.txt` under `gpucheck` with `GpuRender=no`, `GpuVoxelPath=Direct`, `GpuVoxelLayerDepth=Ordered` at scale 1: no `GPU_DIFF` pixel on a vehicle, as in the image-route run (about three one-step pixels a pass, none on a vehicle). A new scene with 48 Kirovs at `GpuVoxelScale=4` (`gpu-voxel-direct.txt`): every `GPU_RENDER` line counts 0 passes that fell back. A `gpuvoxel` unit test: slice n+1 is never behind slice n for any `d` | Recorder, prepared-object cache, model store and walk, `Setup_Object` steps, palette texture, translator and lighting stacks, row depth code (moved to `mapcommon.sh`) | Nothing yet |
| 2 (done) | Composites: hull, turret and barrel in one slice, pieces placed from the blit | `gpu-voxel-compare.txt` turret section with `Ordered`: no `GPU_DIFF` pixel on a vehicle. With `Shared`: a `gpuvoxel` unit test that places two pieces at known offsets and checks which one is nearer, and a `gpu-voxel-shot.txt` still of turreted vehicles at turret facings that differ from the body's, compared by eye | Piece placement arithmetic of `GpuVoxel_Submit` | Nothing: `Ordered` still draws the pieces in the barrel-before-turret order of the software |
| 3 (done) | Shadows as flat cells, stencil once per blit, soft shadows into the mask | `gpu-voxel-shadow.txt` and `gpu-voxel-shadow-bloom.txt`: darkened counts within 5% of 2,148 and 2,002; a new `twice` count in `GPU_VOXEL_SHADOW` of pixels darkened twice by one blit is 0 | `Gpu_Voxel_Walk_Shadow`, `Setup_Shadow`, `Submit_Shadows` | |
| 4 (done) | Translucent, warp and cloak parts with the depth pre-pass; `GpuVoxelCloak` | `cloak-tank.txt` and `chrono-warp-colour.txt` with `Ordered`: `GPU_DIFF` largest step not above the image route's (2 and 1) and mean within its run-to-run spread (161 and 498 in GPU_VOXELS.md) | `Blend_State_For` | |
| 5 (done) | Highlights, drawn into the mask target with its own depth attachment (see "Stage 5 (done)"); parts stay direct while the mask is on | `GPU_MASK` at scale 1 under `gpucheck` with `EmissiveDebugView=yes` and `Ordered`: 0 differing on `gpu-voxel-compare.txt` and `hdr-emissive-scene.txt` with voxel vehicles | `Submit_Voxel_Mask`, `fs_mapmask.sc` structure | `fs_mapmask.sc` reading the voxel image |
| 6 (done) | Face store per layer (no orientation copies), exposed faces only, three visible direction ranges a draw, at `GpuVoxelScale` above 1 with `Shared`. Lighting rows on the GPU and face shading were left out (see "Stage 6 (done)") | `gpuvoxel` unit tests: a single voxel has 6 faces, a solid 3x3x3 block 54, no listed face has a filled neighbour, and the model view places every voxel where the walk does. `gpu-voxel-scale.txt` edge counts at scale 2 and 4 against the image route in the same build | `voxelshade.sh`: the walk's placement, palette and lighting | Nothing: the model store keeps its orientation copies and the per-object `u_vlight` table for scale 1, `Ordered`, shadows and the mask |
| 7 | Splat below a size threshold, faces above; `GpuVoxelScale=0` from the zoom | `bench-large-battle.txt` and `bench-large-battle-zoomed-out.txt` at 3840x2160 meet the budget below; edge counts at zoom 1, 2, 3 | | |
| 8 | Direct by default, then removal of the image route | The regress suite with `GpuVoxelPath=Direct`; `pacing-determinism.txt` hashes identical with the key on and off | | See below |

Later, outside these stages: smooth tilt, the `Voxel` scene depth mode, and GPU terrain. GPU terrain would draw the
tile layer from tile images and their depth data into the same depth attachment in the map's row units, replacing the
uploaded base picture and `s_zbuf`. The voxel row test would then become a hardware test only if voxels wrote row depth,
which conflicts with the slices; that choice belongs to the terrain design.

### Other backends and Direct3D 12

A backend stage runs alongside the stages above, once stage 1's shaders exist. It compiles the map, voxel and scene
shaders for every backend bgfx offers and removes the Direct3D 11 check from the `Start` functions in `code/gpumap.cpp`
and `code/gpumapvoxel.cpp`. Linux needs Vulkan or OpenGL and macOS needs Metal, so this work is required whatever
Direct3D 12 turns out to be worth. Today `Renderer=2` (Direct3D 12) starts the game, but the map pass is drawn in
software there, so Direct3D 12 is slower than Direct3D 11 until this stage lands.

We expect little gain from Direct3D 12 at today's draw counts. It lowers the CPU cost of each draw command, and a pass
today sends one batch of quads plus one draw for each voxel part, about 100 to 200 draws. Most of the frame goes to the
software terrain layer, its upload and the recording (Performance budget below), which no API change speeds up. The
gain should grow once GPU terrain and direct voxels raise the draw count, and once the render thread
([RENDER_THREAD.md](RENDER_THREAD.md)) can record draws in parallel, which Direct3D 12 and Vulkan allow.

The check is a measurement, made by a person because automated runs use only the default renderer:
`bench-large-battle.txt` and `bench-late-game.txt` at 3840x2160 with `Renderer=1` and `Renderer=2`, comparing mean and
p95 draw, `present.gputime` and draws a second, followed by a play session on each. Direct3D 12 becomes the Windows
default only if it is faster on both benchmarks and the play session shows no driver fault.

### Stage 1 (done)

**State.** Stage 1 is built and committed on branch `gpu-direct-voxels`, together with the hard shadows of stage 3.
With `GpuVoxels=yes`, `GpuVoxelPath=Direct` and the emissive mask off, hull-only vehicles, aircraft and other plain
voxel objects, and their shadows while `SoftShadows` is off, are drawn straight into the scene. At scale 1 with
`GpuVoxelLayerDepth=Ordered` the pass differs from the software exactly where the image route's does, and 48 Kirovs at
`GpuVoxelScale=4` and 3840x2160 stay on the GPU at full scale in every pass. Before the overflow fix the image route drew
3,579 of 3,600 of those passes in software; since it, the image route draws them on the GPU with the Kirovs at scale 1.
Stage 2 (below) added turreted vehicles. Stage 5 lets the route reach a player with Glow or HDR Enhanced highlights
on; before it the mask kept every part on the image route.

What we built, in the order of the plan:

1. **Keys.** `GpuVoxelPath` (`Image`, `Direct`, default `Direct`) and `GpuVoxelLayerDepth` (`Shared`, `Ordered`, default
   `Shared`) are name-list entries in `code/optionsettings.cpp` with no row, `OptionsClass` members and enums in
   `code/options.h`, and are passed on with `GpuVoxel_Set_Path` beside `GpuDraw_Set_Voxels` in `code/gscreen.cpp`.
   The manual has their key pages and `manual/changes/added-gpuvoxelpath.md`; `tests/settingregistry` pins their order
   and round trip.
2. **Routing.** `GpuVoxel_Direct_Parts` (`code/gpumapvoxel.cpp`) marks two kinds of part direct while the key is
   `Direct`, the mask is off and the direct program built: one prepared object without shadows that only copy blits
   show, and one prepared shadow that only shadow blits show, the second only without soft shadows. A part no blit
   shows stays on the image route, so `gpucheck` still compares it. A pass with more than `GPU_VOXEL_SLICES` (4,096)
   direct object blits, or more than 255 direct shadow blits (`GPU_VOXEL_STENCILS` since stage 4), sends those parts to the
   image route for that pass rather than splitting the view. `GpuVoxel_Fits` and `GpuVoxel_Submit` skip direct parts,
   which get no tile (`GpuVoxelTile::Direct`), and `GpuVoxel_Begin_Check` leaves them out of the `GPU_VOXEL` lines.
3. **Target.** `Size_Target` (`code/gpumap.cpp`) adds the D24S8 attachment while `GpuVoxelPath=Direct` is set, whether
   or not a pass has a direct part, so a pass with none does not remake the targets. `Submit_Draws` clears depth to 1
   and stencil to 0 on `VIEW_MAPDRAW` when the pass has a direct part; the wave views keep both.
4. **Model data.** Direct layers go through `Load_Models` in the same call as the image parts' layers, so one emptying
   of the model texture cannot drop either. `Set_Layer_Depths` gives each layer its depth: `Gpu_Voxel_Ordered_Depth`
   or `Gpu_Voxel_Shared_Depth` (`code/gpuvoxel.cpp`). `Shared` reads the view Z of the layer's walk origin and of its
   corners along the three walk axes through `VoxelRenderOrientations`, the corners `VoxelLibrary::Setup_Object` takes
   its 2D steps from, and counts the largest Z as nearest, because the software depth buffer keeps the larger value
   (`code/voxlib.cpp`). The range is the part's `BoxCorner` Z range; a cube corner up to half a voxel outside it is
   clamped to the end of the slice.
5. **Draws.** For a direct part's blit, `Submit_Draws` calls `GpuVoxel_Draw_Direct` with the values of the blit's own
   quad (`Direct_Blit`), so the row depth test, translator row and lighting band are the ones the image route's quad
   carries. An object's layers write depth and test `LEQUAL` in the blit's slice. A shadow's layers neither test nor
   write depth; they darken through the blend of `Blend_State_For(KIND_DARKEN)` with stencil test `NOTEQUAL` and
   replace, at the next of the pass's values 1 to 255, so each pixel darkens once per shadow blit.
6. **Shaders.** `mapcommon.sh` holds the stores and the row test and translator code that `fs_mapdraw.sc` and
   `fs_voxelscene.sc` share; `voxelwalk.sh` holds the walk that `vs_mapvoxel.sc` and `vs_voxelscene.sc` share.
   `vs_voxelscene.sc` divides each position by the scale, because the map view's projection maps area pixels, and reads
   the model and palette textures at stages 11 and 12, because bgfx binds a stage to both shaders of a program and
   `fs_voxelscene.sc` uses stages 1 to 5. Rebuilt with the same `shaderc`, `vs_mapvoxel.bin.h` is byte-identical
   after the move; `fs_mapdraw.bin.h` grew from 24,359 to 24,371 bytes, and the image-route checks below did not change.

**Checks.**

| Check | Result |
|---|---|
| `gpuvoxel` unit test | The three new checks pass: slice n + 1 is never behind slice n for any `d`, also as stored in 24 bits; `Ordered` depth never moves back along a walk of 20,006 voxels; `Shared` puts a nearer voxel in front |
| `settingregistry` | 71 checks pass |
| `gpu-voxel-compare.txt` under `gpucheck`, `GpuRender=no`, scale 1, `Direct` and `Ordered` | The 83 `GPU_DIFF` lines take the same values as the image route's 82: 0 to 6 differing pixels a pass, none more than one step, at the same first pixels. 565 `GPU_VOXEL` lines against 1,594, because a direct part has no tile to compare |
| The same with `Shared` | 21 to 346 differing pixels a pass, steps up to 60: at scale 1 a voxel is a one or two pixel stamp, and stamps hide each other by depth where the walk would overwrite. Recorded, not a check |
| `gpu-voxel-shadow.txt` with `Direct` | 2,148 darkened pixels at scale 1 and 2,002 at scale 2, the image route's counts |
| The 8 `gpu-voxel-*` regress scripts | The 7 recorded ones `same` with the keys at their defaults; `gpu-voxel-direct.txt` new, its log in `tests/autotest/gpu-voxel-direct.log` |
| `gpu-voxel-direct.txt` (48 Kirovs, scale 4, 800x600) | `Direct`: 0 of 1,200 passes fall back. `Image`, before the overflow fix: 1,195 fall back |
| 48 Kirovs at scale 2, zoomed, `Shared` against `Ordered` | 15 of 7,731,216 pixels differ |

The "the voxel parts cannot be drawn" line is not a fallback count: a pass whose parts do not fit an empty image is
refused by `Pass_Fits` without a line, and the line appears only when `Submit_Pass` fails afterwards. The checks above
count fallbacks from `GPU_RENDER` lines.

**Measurements.** 48 Kirovs (`scratch-agent/voxel-kirov/kirovs.txt` with `perflog 1`) at 3840x2160, uncapped game
speed, `HDRHighlights=Classic` and `Bloom=no` so that the mask is off; with this PC's default highlights the mask is on
and, until stage 5, both routes were the image route. Passes are the first six `GPU_RENDER` lines; times are means of the per-second
`Perf` lines after the first three.

| Route | Scale | Passes on the GPU | Draws a second | Mean / p95 draw | `draw.gpumap` | `present.gputime` |
|---|---:|---:|---:|---:|---:|---:|
| Image | 1 | 3,600 of 3,600 | 261 | 3.67 / 4.12 ms | 0.01 ms | 3.48 ms |
| Image | 2 | 21 | 34 | 29.11 / 31.33 ms | 26.68 ms | 0.88 ms |
| Image | 4 | 21 | 33 | 30.24 / 36.43 ms | 27.17 ms | 1.03 ms |
| Direct | 1 | 3,600 | 259 | 3.73 / 4.12 ms | 0.01 ms | 4.14 ms |
| Direct | 2 | 3,600 | 251 | 3.85 / 4.13 ms | 0.01 ms | 4.14 ms |
| Direct | 4 | 3,600 | 262 | 3.64 / 4.13 ms | 0.01 ms | 4.14 ms |

These runs predate the overflow fix. The image route's fallback passes spend their time rendering the voxels in software
(`draw.gpumap`). On the direct route the main thread costs what the image route costs at scale 1. Presents stay at 240 a
second in every run because `VSync` and the default `FrameRateLimit` pace them to this PC's 240 Hz display, so a
`present.gputime` of 4.14 ms there is that limit (4.17 ms a present), not the cost of the pass.

**GPU cost without the present limit.** The same scene with `VSync=no` and `FrameRateLimit=0` (presents at every draw),
after the overflow fix and with stage 2 in; times are means of the per-second `Perf` lines after the first three, 5 to
17 lines a run.

| Route | Scale | Draws and presents a second | Mean / p95 draw | `present.gputime` mean / p95 | Kirovs drawn below the scale |
|---|---:|---:|---:|---:|---|
| Image | 1 | 1,397 | 0.64 / 0.94 ms | 0.20 / 0.26 ms | |
| Image | 2 | 1,381 | 0.65 / 0.92 ms | 0.32 / 0.37 ms | All, at 1 |
| Image | 4 | 1,072 | 0.86 / 1.17 ms | 0.89 / 1.27 ms | All, at 1 |
| Direct | 1 | 1,302 | 0.69 / 0.96 ms | 0.41 / 0.47 ms | |
| Direct | 2 | 1,079 | 0.85 / 1.19 ms | 0.88 / 1.27 ms | None |
| Direct | 4 | 576 | 1.65 / 2.09 ms | 1.70 / 2.03 ms | None |

The direct route draws every voxel of the 48 Kirovs and their shadows in every pass, where the image route reuses its
tiles in this still scene and draws nothing, so the difference in `present.gputime` is the cost of drawing the voxels:
about 0.2 ms at scale 1, 0.6 ms at scale 2 and 0.8 ms at scale 4. At scales 2 and 4 the image route shows the Kirovs at
scale 1, so its picture is the less sharp one. The whole pass at scale 4 takes 1.7 ms of GPU time, under the 4.17 ms a
picture has at 240 a second. A moving army changes the image route's side, because its parts must be drawn again
whenever their pose changes.

**Changes to the design.**

- The hard shadows of stage 3 came forward into stage 1. Without them the 48 Kirovs still fall back at scale 4: their 49
  shadow parts of 146 by 77 pixels need 584 by 308 tiles, and fewer than 20 of those fit the image.
- The slice and stencil limits route parts to the image for the pass instead of splitting the view.
- The depth attachment follows the key, not the pass.
- The vertex shader divides by the scale and samples at stages 11 and 12 (step 6).

**Checks made after stage 1.**

- **Scale 3.** The vertex shader divides a corner's position by the scale. At scale 3 a corner can lie halfway between
  two of the rasterizer's 1/256-pixel steps, and the inexact division then decided which step it took: window shots of
  the `gpu-voxel-scale.txt` scene with `Ordered` differed from the image route's in 9 and 37 window pixels of the map area
  at zoom 1.5 and 2, while scales 2 and 4 had none. `vs_voxelscene.sc` now puts each corner on a step first, rounding a
  tie to even, and all three scales give the image route's map area exactly at both zooms.
- **Edge counts.** Window shots of the same scene at 800x600, map area only, counting pixels whose colour differs from
  the right or lower neighbour by more than 24 in a channel (terrain included). `Ordered` gives the image route's counts
  exactly, since the pictures are equal; `Shared` stays within 0.1% of them.

  | Scale | Zoom | Image route and `Ordered` | `Shared` |
  |---:|---:|---:|---:|
  | 2 | 1.5 | 21,299 | 21,289 |
  | 2 | 2 | 34,228 | 34,218 |
  | 4 | 1.5 | 26,797 | 26,793 |
  | 4 | 2 | 34,409 | 34,384 |

- **Determinism.** `pacing-determinism.txt` with `GpuRender=yes`, `GpuVoxels=yes` and `GpuVoxelPath=Direct`, then
  `Image`: `compare_digests.py --hash` reports the 1,500 hash lines identical, and neither log has a `syncrec outside
  frame logic` line. A `syncrec` replay comparison is not done.

### Stage 2 (done)

**State.** Built and committed on branch `gpu-direct-voxels`. With the direct path on and the emissive mask off, a
turreted vehicle composed from hull, turret and barrel is drawn straight into the scene when every blit that shows it is
a copy. Stage 3 (below) finished the shadows.

What we built:

1. **Routing.** `GpuVoxel_Direct_Parts` takes any part whose pieces have no shadow layers and whose blits are all
   copies, a composite included; shadow parts stay single pieces. A cloaked or warping vehicle's blit is translucent, so
   its composite stays on the image route.
2. **Placement.** No new drawing code was needed. `GpuVoxel_Draw_Direct` already placed each piece at the blit's origin
   plus `piece.Dest - part.Area`, clipped it to the piece's bounds and the blit's rectangle, and read the palette
   texture's second block for a composite, the arithmetic `GpuVoxel_Submit` uses inside a tile. A composite is one blit,
   so it takes one depth slice.
3. **Depth.** With `Ordered` the layers keep the software's order across the pieces (hull, then barrel and turret in the
   order `UnitClass::Unit_Draw_Voxel` composes them), so that order is still needed on this route. With `Shared` the
   pieces share one view depth range, which `Gpu_Voxel_Widen_Depth_Range` builds over every layer of the part. Their
   corners are in one view space because `Unit_Draw_Voxel` puts the turret offset, the turret's turn and the barrel's
   pitch into each piece's matrix; only the turret bob is a pixel shift, which moves a piece on screen and not in depth.

**Checks.**

| Check | Result |
|---|---|
| `gpuvoxel` unit test | New: "shared depth orders two pieces by their placement", a hull and a turret placed nearer by its matrix, passes |
| `gpu-voxel-compare.txt` under `gpucheck`, `GpuRender=no`, scale 1, `Ordered` | The 75 `GPU_DIFF` lines take the values of the image route's 82 in a run made beside it: 0 to 6 differing pixels a pass, none more than one step, at the same three first pixels. No composite `GPU_VOXEL` line, against 560 on the image route, because every composite was direct |
| The same with `Shared` | 0 to 1,230 differing pixels a pass, steps up to 63, where layers and pieces hide each other by depth. Recorded, not a check |
| `gpu-voxel-turrets.txt` (new), scale 2, zoom 2 | `Ordered` gives the image route's map pixels exactly. `Shared` compared by eye: turrets and barrels sit in place at turret facings other than the body's, and the differences are single pixels along edges where layers meet |
| 48 Kirovs at 3840x2160, scale 2 and 4 | 0 passes fall back on either route |

### Stage 3 (done)

**State.** Built and committed on branch `gpu-direct-voxels`. With the direct path on and the emissive mask off, every
voxel shadow takes the direct route: with hard shadows it darkens the scene once per blit through the stencil (built in
stage 1), and with `SoftShadows` its layers are drawn into the shadow mask. Stage 5 (below) came next.

What we built:

1. **Routing.** `GpuVoxel_Direct_Parts` takes shadow parts under soft shadows too. A soft shadow takes no stencil value,
   so the limit of 255 such blits a pass (`GPU_VOXEL_STENCILS`) applies only to hard shadows.
2. **Soft shadows.** `Submit_Shadows` (`code/gpumap.cpp`) calls `GpuVoxel_Draw_Direct` for a direct shadow blit in place
   of its quad, at scale 1 whatever `GpuVoxelScale` is, because the mask is map-sized. The quad's values carry the shadow
   mask kind, which `fs_voxelscene.sc` writes as 1 after the blit's row depth test, as `fs_mapdraw.sc` does from a tile.
   The mask is written, not blended, so overlapping cells need no stencil.
3. **The `twice` count.** The `GPU_VOXEL_SHADOW` line counts the pixels inside only one shadow's rectangle that hold the
   ground darkened two or three times, computed the way the darken blend rounds in the 8-bit target, to within one step
   in each channel (`Is_Darkened_Again`). A looser first test, a quarter of the ground's brightness to within 8 steps,
   counted 86 pixels on the image route too, whose tile darkens each pixel once, so it took other pixels for shadow.

**Checks.** `gpu-voxel-shadow.txt` with `GpuVoxelPath` and `SoftShadows` set as listed:

| Shadows | Scale | Image route: darkened / twice | Direct route: darkened / twice |
|---|---:|---:|---:|
| Hard | 1 | 2,148 / 0 | 2,148 / 0 |
| Hard | 2 | 2,002 / 0 | 2,002 / 0 |
| Soft | 1 | 1,531 / 0 | 1,531 / 0 |
| Soft | 2 | 1,433 / 0 | 1,522 / 0 |

- With the stencil taken out, the direct route's hard shadows give 125 darkened and 518 twice at scale 1, so the count
  catches a shadow that darkens a pixel more than once. At scale 2 the same build gave 2,002 and 0: the count reads the
  centre of each block of finer pixels, which those overlaps missed.
- Under soft shadows at scale 2 the image route samples each part's tile at the scale it was drawn, while the direct
  route draws the mask at scale 1, the mask both routes give at scale 1; the counts differ by 6%.
- The last Perf line of the soft shadow run at scale 1 counts 762 voxel parts, all direct.
- `gpu-voxel-direct-shadow.txt` and `gpu-voxel-direct-soft.txt` (new, logs in `tests/autotest/`) record the direct
  route's hard shadows at scale 1 and soft shadows at scale 2. `gpu-voxel-shadow.txt` and `gpu-voxel-shadow-bloom.txt`
  log the new `twice` field, 0, beside the counts recorded before (new logs in `tests/autotest/`).
- The other `gpu-voxel-*` regress scripts are `same`. 48 Kirovs at 3840x2160 with `Bloom=yes`: 0 of 600 passes fall back
  at scale 2 and 4. The mask keeps them on the image route until stage 5, which shows them below the scale.
- `pacing-determinism.txt` with `GpuRender=yes`, `GpuVoxels=yes`, `SoftShadows=yes`, `Bloom=no` and
  `HDRHighlights=Classic`, `GpuVoxelPath=Direct` against `Image`: the 1,500 hash lines are identical.

### Stage 5 (done)

**State.** Built and committed on branch `gpu-direct-voxels`. Direct parts keep the direct route while the emissive mask
is on (`HDRHighlights=Enhanced` with an HDR display, or `Bloom`), which this PC's default settings turn on, and write
their highlight strengths into the mask; turreted vehicles get highlights. Stage 4 (below) came next.

We built the alternative the design names for the mask, not the RG8 scene attachment: the mask target gets its own
D24S8 attachment, at map size, and each direct part's layers are drawn into it a second time at scale 1. No other map
shader changes, and the scene target keeps one colour attachment.

1. **Routing.** `GpuVoxel_Direct_Parts` no longer takes the mask into account.
2. **The mask pass.** `Submit_Voxel_Mask` (`code/gpumap.cpp`) keeps its order: a copy of the CPU plane, then one quad
   per voxel blit in blit order. An image part's quad still reads its tile. A direct part's quad writes 0 wherever the
   plane holds `GPU_VOXEL_MASK_MARK` (`fs_mapmask.sc`), and the part's layers follow at scale 1 in the blit's depth
   slice, depth test `LEQUAL`, writing the voxel's strength where the plane holds the mark (`fs_voxelscene.sc`, kind
   16). Within a part each pixel takes the voxel depth puts in front, the last in the walk with `Ordered`, which is
   what the tile holds; across parts a later blit's zeroing and slice put it over an earlier one, as its quad does on
   the image route. The tile holds every voxel's strength whatever the map's depth test says, so these draws skip
   that test, and an index 0 voxel writes its strength as it does in the tile.
3. **Composites.** The scratch surface has no plane, so a composite's blit zeroes the target's plane. On the direct
   path `GpuDraw_Composite_End` (`Mark_Composite_Mask`, `code/gpudraw.cpp`) fills the rectangles of the composite's
   blits with the mark afterwards. A composite on the image route then gets a zeroing quad, and a pass drawn in software
   turns the marks into 0, so only a direct composite shows highlights. A pass `gpucheck` compares draws no composite
   highlights, because the software it is compared with has none.
4. **Scale.** The mask is map-sized, so a direct part's strengths are those of scale 1 whatever `GpuVoxelScale` is,
   where the image route reads the centre of each block of its tile.

**Checks.**

| Check | Result |
|---|---|
| `gpu-voxel-compare.txt` under `gpucheck`, `GpuRender=no`, `EmissiveDebugView=yes`, `Ordered`, scale 1 | 90 `GPU_MASK` lines, all 0 differing (95,602 masked pixels). The image route in the same setup: 86 lines, all 0. The `GPU_DIFF` lines on both routes: up to 6 differing pixels a pass, none more than one step |
| `hdr-emissive-scene.txt` with `gpucheck`, a harvester, an MCV, a Kirov and an Orca added, and no window shots | 583 `GPU_MASK` lines, all 0 differing (929,720 masked pixels). The image route: 572 lines, all 0 |
| Before composites were left out of checked passes | The compare scene's passes differed by up to 2,148 pixels, every one a turreted vehicle's highlight over a mask the software leaves at 0 |
| `gpu-voxel-turrets.txt` with `EmissiveDebugView=yes`, scale 2, `Shared` | By eye: on the direct route the turreted vehicles show highlights on hull and turret, in the vehicles' outlines; on the image route only the two hull-only vehicles do. `gpu-voxel-bloom.txt` under `Bloom` shows the Grizzly tank glowing on the direct route only |
| `gpu-voxel-direct-bloom.txt` (new, log in `tests/autotest/`): 48 Kirovs at scale 4 under `Bloom`, 800x600 | Direct: 0 fallback, 0 reduced. Image: 0 fallback, 28,416 and 54,912 reduced in the two lines |
| 48 Kirovs at 3840x2160, `Bloom=yes`, scale 2 and 4 | 0 of 600 passes fall back, 0 reduced (stage 3: 13,536 and 23,912 reduced) |
| The other `gpu-voxel-*` regress scripts | `same`, except that the logs of `gpu-voxel-shadow.txt` and `gpu-voxel-shadow-bloom.txt` gain stage 3's `twice` field; the direct route's scripts match their logs in `tests/autotest/` |
| `pacing-determinism.txt`, `GpuRender=yes`, `GpuVoxels=yes`, `Bloom=yes`, `SoftShadows=yes`, `Direct` against `Image` | The 1,500 hash lines are identical |

**GPU cost.** 48 Kirovs (`kirovs.txt` with `perflog 1`, quitting at frame 6,000) at 3840x2160, uncapped game speed,
`Bloom=yes` with this PC's other defaults, `VSync=no` and `FrameRateLimit=0`, so presents follow every draw. Times are
means of the per-second `Perf` lines after the first three, 4 to 13 lines a run.

| Route | Scale | Draws and presents a second | Mean / p95 draw | `present.gputime` mean / p95 | Kirovs drawn below the scale |
|---|---:|---:|---:|---:|---|
| Image | 1 | 802 | 1.17 / 1.45 ms | 0.37 / 0.44 ms | |
| Image | 2 | 805 | 1.17 / 1.45 ms | 0.69 / 0.86 ms | All, at 1 |
| Image | 4 | 553 | 1.74 / 2.21 ms | 1.75 / 2.26 ms | All, at 1 |
| Direct | 1 | 719 | 1.32 / 1.60 ms | 0.86 / 1.09 ms | |
| Direct | 2 | 660 | 1.44 / 1.86 ms | 1.29 / 1.89 ms | None |
| Direct | 4 | 370 | 2.63 / 2.86 ms | 2.67 / 2.91 ms | None |

The direct route costs about 0.5 ms more GPU time a present than the image route at scale 1, 0.6 ms at scale 2 and
0.9 ms at scale 4, where the image route reuses its tiles in this still scene and shows the Kirovs at scale 1 above
scale 1. Without the mask (stage 2's table) the difference was 0.2, 0.6 and 0.8 ms, so the second draw of the layers
into the mask adds about 0.3 ms at scale 1 and 0.1 ms or less at scales 2 and 4, where the scene's own draws grow with
the scale and the mask's, drawn at scale 1, do not. The whole pass at scale 4 takes 2.7 ms of GPU time, under the
4.17 ms a picture has at 240 a second.

### Stage 4 (done)

**State.** Built and committed on branch `gpu-direct-voxels`. A part whose blits are translucent (25%, 50% or 75%; the
warp fade draws at 50%) takes the direct route, a composite included. `GpuVoxelCloak=Ripple` adds a ripple to a cloaked
object; it is described after the checks. Stage 6, the face store, came next.

What we built:

1. **Routing.** `GpuVoxel_Direct_Parts` takes a part that only copy or translucent blits show. A translucent blit takes
   a stencil value, as a hard shadow does, from the same 255 a pass (`GPU_VOXEL_STENCILS`, formerly
   `GPU_VOXEL_SHADOW_STENCILS`); when a pass needs more, its parts with such blits take the image route.
2. **Depth pre-pass.** For a translucent blit `Submit_Draws` draws the part's layers twice in the blit's slice
   (`Scene_Blit`, `Depth_Pre_Pass` in `code/gpumap.cpp`): first writing depth only, test `LEQUAL`, then with depth test
   `EQUAL`, no depth write, the blend `Blend_State_For` gives the blit's kind, and a stencil value that passes each pixel
   once. `fs_voxelscene.sc` writes the translucent kinds through `Translucent_Output`, which moved from `fs_mapdraw.sc`
   into `mapcommon.sh`; `fs_mapdraw.bin.h` rebuilt byte-identical.
3. **Equal depths.** With `Ordered`, a slice has 4,096 depth steps in the 24-bit buffer, so in a part of more voxels
   neighbours in the walk share a depth. A copy's `LEQUAL` leaves the later of them, as the walk does, but the first
   draw to pass a pixel once would leave the earlier. The blend draw therefore takes the layers and their voxels last
   first (`GpuVoxelDirectBlit::LastFirst`, `u_vdepth[2]` in `vs_voxelscene.sc`), keeping each voxel's corners in their
   order so that a line covers the same pixels. Before this, `chrono-warp-colour.txt` gave 25 pixels more than a step
   off, up to 13 steps, on the warping vehicles; with 64 slices instead of 4,096 the same build gave none.

**Checks.** `gpucheck` copies of the scripts, `GpuRender=no`, `GpuVoxelLayerDepth=Ordered`, scale 1, each route run
beside the other:

| Script | Route | Passes | `GPU_DIFF` mean / most differing | Largest step |
|---|---|---:|---:|---:|
| `chrono-warp-colour.txt` | Image | 45 | 471 / 2,080 | 1 |
| `chrono-warp-colour.txt` | Direct | 47 | 420 / 2,080 | 1 |
| `cloak-tank.txt` (cloak test mod) | Image | 160 | 52 / 1,357 | 2 |
| `cloak-tank.txt` (cloak test mod) | Direct | 181 | 49 / 1,343 | 2 |

- The warping Rhino and Ore Truck are direct: frames 70 to 119 have no `GPU_VOXEL` line for them on the direct route and
  52 on the image route.
- In `cloak-tank.txt` the cloaked Grizzly's composite has a `GPU_VOXEL` line, so no voxel blit showed it, in 23 of the
  38 passes with differing pixels on the direct route and 19 of 34 on the image route. We have not established whether
  the remaining passes drew it direct, so this scene shows that cloaking changes nothing on the direct route, not that a
  cloaked part is drawn right.
- `gpu-voxel-direct-warp.txt` (new, log in `tests/autotest/`): the warp scene on the GPU with `Bloom` at scale 2; both
  routes have the same 10 fallback passes, those of the chronosphere effect.
- The `gpu-voxel-*` scripts, played one at a time through `regress.py`'s own `play` while a nightly run held the
  regress slots: unchanged apart from stage 3's `twice` field, and the direct route's scripts match their logs in
  `tests/autotest/`.
- 48 Kirovs at 3840x2160, `Bloom=yes`, scale 2 and 4: 0 of 600 passes fall back, 0 reduced.
- `pacing-determinism.txt` with `GpuRender=yes`, `GpuVoxels=yes`, `Bloom=yes` and `SoftShadows=yes`, `Direct` against
  `Image`: the 1,500 hash lines are identical.

#### Cloak ripple

With `GpuVoxelCloak=Ripple`, the voxel blits of an object in the last stage of cloaking (`VISUAL_RIPPLE`, which the software marks with
`SHAPE_PREDATOR`) draw with a ripple. `GpuDraw_Voxel_Cloak` (`code/gpudraw.cpp`), called where `techno.cpp` and `unit.cpp` pick the blitter, records
on the blit whether the object is cloaked (`GpuBlit::Cloaked`) and its predator offset (`GpuBlit::Predator`), because
`GpuDraw_Voxel_Blit_Flags` drops the flag. For such a blit `Submit_Draws` starts the next wave view with a copy of the
target (`Begin_Snapshot_View`, shared with the sonic waves), binds the copy as `s_snapshot`, and draws the part with no blend:
`fs_voxelscene.sc` (`Cloak_Ripple`) adds the translucent source term to the copy's colour at a pixel shifted by up to two
map pixels along a wave of the object's offset and the time.

- **Views.** A rippled cloak and a sonic wave each take one of the eight wave views, so a pass draws the ripples that fit
  beside its waves and the remaining cloaked objects plainly translucent.
- **Differs from the software.** The software draws no ripple, so there is nothing to match; the ripple is a new look and
  reads the picture as it was just before the object, which includes every object drawn earlier.
- **Only `VISUAL_RIPPLE`.** The game draws it only for an object that is not the player's own and is between 192/256 and
  255/256 of the way to cloaked (`TechnoClass::Visual_Character`), which lasts a few frames. A cloaked object an owner or ally
  sees (`VISUAL_SHADOWY`) and a warping object stay translucent.
- **Check.** Under `gpucheck`, `GpuMap_Check` logs `GPU_VOXEL_CLOAK frame F direct D image I ripple R` for a pass that has
  cloaked voxel blits: D drawn direct, I through the voxel image, R with the ripple. `cloak-tank.txt` with the cloak test mod,
  `GpuVoxelPath=Direct`, `Ordered`, scale 1, and a `follow MTNK` line so that the camera shows the tank, gave 45 such lines,
  all `direct 1 image 0`: the cloaked tank was drawn direct in every pass that showed it. The tank was off-screen in the
  script as recorded, which is why the earlier check proved nothing; the script has no `follow` line yet. With the tank in view, `GPU_DIFF` gave 173 passes, 110 differing, mean 253 and at most 1,914
  differing pixels, largest step 2 on the direct route; the image route gave 174, 111, 246, 1,879 and 2.
- **Ripple screenshot.** To see the ripple, a debug build drew the player's own cloaked tank with the predator offset
  (the game gives it none); a shot with `Ripple` differed from one with `Translucent` in the tank's pixels (1,168 and 654
  pixels more than 6 apart in the two cloaked shots, none after it decloaked). That build change is not committed.

### Stage 6 (done)

**State.** Built on branch `direct-voxels-faces`. With `GpuVoxelPath=Direct`, `GpuVoxelLayerDepth=Shared` and
`GpuVoxelScale` above 1, the object layers of a direct part draw as the exposed faces of their voxels, where they drew
each voxel's cube outline before. Scale 1, `Ordered`, shadows and the mask draws are unchanged. Fully zoomed out at
3840x2160 the faces cost the same as the outlines (table below). **Next step:** stage 7.

What we built:

1. **The face list.** `Gpu_Voxel_Faces` (`code/gpuvoxel.cpp`) lists every voxel face of a layer that has no voxel
   beside it, sorted into the six directions. The list is the same in every orientation, so a layer has one list where
   the model store holds a walk for each orientation. A voxel that a column's runs place above the layer's height is
   left out. The VXLs of Yuri's Revenge have 2,098,462 exposed faces in 242 layers, the largest layer 88,186; those of
   Mental Omega have 9,942,900 in 1,053 layers, the largest 163,812.
2. **The face store.** A 2048x2048 RG32U texture holds 4,194,304 faces. `Load_Faces` (`code/gpumapvoxel.cpp`) lists a
   layer's faces in the first pass that draws it as faces and keeps them under the voxel library, the layer and
   whether the layer is lit. A pass whose new layers do not fit the space left empties the store first. A pass whose
   layers do not fit an empty store draws them as outlines, and lists their faces again in every such pass.
3. **The draw.** `Gpu_Voxel_Model_View` gives the three directions that face the viewer in the layer's orientation, and
   the draw submits those three ranges, which hold half of the layer's exposed faces. `vs_voxelface.sc` makes six
   vertices a face from `gl_VertexID` and the face's record. It shares the walk's placement, palette and lighting
   through `voxelshade.sh`, and each vertex takes the `Shared` depth of its own corner.
4. **The counter.** `gpu voxel faces` in the `Perf` count line is the number of faces the direct draws submitted.

What differs from the plan in the stage table:

- **Lighting rows stay on the CPU.** A face draw reads the `u_vlight` table the walk reads, so a face has the colour
  the walk gives its voxel, and the planned comparison of GPU lighting rows with `Precalculate_Normal_Lookup` has
  nothing to test. A texture of lighting rows remains a candidate for stage 7: the table is a 64-vector uniform set
  for every layer draw.
- **No face shading.** A face takes its voxel's colour whatever its direction.
- **The model store stays.** Scale 1, `Ordered`, shadows and the mask draws use its walks, and `Load_Models` still
  loads the walk of a layer that draws as faces. This stage deletes nothing.

**Checks.**

| Check | Result |
|---|---|
| `gpuvoxel` unit tests | Pass, with three new ones: "a voxel has 6 faces and a solid 3x3x3 block 54", "the face list is the layer's exposed faces", "the model view places voxels where the walk does" |
| `gpu-voxel-scale.txt` at 800x600, edge counts as in stage 1 over the window without its right 168 pixels | Table below. With `Shared`, the faces are 5 and 11 edges below the image route at scale 2 (zoom 1.5 and 2) and 1 and 19 below at scale 4; stage 1's outlines were 10, 10, 4 and 25 below. Stage 1's absolute counts belong to an older window layout and do not compare |
| The same shots, pixels that differ from the image route's | Scale 2: 44, 43 and 53 at zoom 1, 1.5 and 2. Scale 4: 40, 45 and 74 |
| `regress.py --only "gpu-voxel\|voxel\|cloak"` | 23 scripts, all `same` |
| Unit tests, the two test missions and the netsync run of the branch check | 119 tests pass, both missions won, in sync over 5,266 ticks |
| `gpu voxel faces` in the bench below | A mean of 327,095 faces a draw over 91 direct parts at scale 2; 0 at scale 1 |

| Route | Scale | Edges at zoom 1.5 | Zoom 1 | Zoom 2 |
|---|---:|---:|---:|---:|
| Image | 2 | 30,014 | 74,951 | 44,275 |
| Direct, `Shared` (faces) | 2 | 30,009 | 74,950 | 44,264 |
| Direct, `Ordered` (outlines) | 2 | 30,013 | 74,951 | 44,275 |
| Image | 4 | 35,669 | 75,153 | 44,250 |
| Direct, `Shared` (faces) | 4 | 35,668 | 75,151 | 44,231 |

**Zoomed out at 4K.** `bench-large-battle-xmp04-zoomed-out.txt` as in "Performance budget": XMP04T8 at zoom 0.50,
3840x2160, uncapped, `GpuVoxelSmooth=yes`. Times are means of the per-second `Perf` lines after the first five. Another
game was using the GPU while we measured: 8 of the 20 runs had a mean draw of 69 to 141 ms and are left out, and the
table holds the two least disturbed rounds. No pass fell back to software in any run.

| Keys | Draws a second | Mean / p95 draw | `draw.gpumap` | `present.upload` | `present.gpu` | `present.gputime` |
|---|---:|---:|---:|---:|---:|---:|
| `GpuVoxels=no` | 628 | 1.16 / 2.45 ms | 0.09 ms | 0.29 ms | 0.23 ms | 0.66 ms |
| Direct, scale 2, before this stage (outlines) | 319 | 2.69 / 5.67 ms | 0.40 ms | 0.72 ms | 0.72 ms | 1.57 ms |
| Direct, scale 2 (faces), two runs | 310, 309 | 2.78 / 5.72, 2.78 / 5.91 ms | 0.40, 0.41 ms | 0.74 ms | 0.79 ms | 1.82, 1.64 ms |
| Direct, scale 1, two runs | 314, 317 | 2.72 / 5.81, 2.71 / 5.63 ms | 0.40, 0.41 ms | 0.73, 0.72 ms | 0.74 ms | 1.10, 1.10 ms |

- **Faces against outlines.** The two cost the same at this zoom within the spread between runs, on the CPU and on the
  GPU. Two more rounds under heavier load agree: 255 and 241 draws a second with outlines, 263 with faces.
- **Against the budget.** `draw.voxels` is 0.12 ms on every direct row, within its 0.2 ms. `present.gputime` meets its
  1.5 ms at scale 1 and misses it at scale 2. `draw.gpumap` is 0.31 ms above `GpuVoxels=no`, over its 0.2 ms.
- **Where the direct route's time goes.** The faces at scale 2 take 1.62 ms a draw more than `GpuVoxels=no`:
  `present.gpu` 0.56 ms, `present.upload` 0.45 ms, `draw.gpumap` 0.32 ms, `draw.objects` 0.21 ms and `draw.voxels`
  0.08 ms, for about 92 parts. No texture is uploaded in these runs (`uploads` is 0). `present.upload` grows because
  the presenter submits the map pass there: `draw.gpumap` times `GpuDraw_End`, which checks that the pass fits and
  keeps it, and `GpuMap_Texture`, called from `Backend_Present`, then submits its draws, the voxel layers among them
  and, while the emissive mask is on, a second time for the mask. `present.gpu` is `bgfx::frame` carrying those draws out.
- **For stage 7.** The cost at this zoom follows the number of layer draws and their vertices, which faces do not
  lower. Stage 7's splats and its scale from the zoom apply to this view. For the three CPU phases the candidates are
  fewer and cheaper layer draws: the lighting rows in a texture, and no fit check for a pass that needs no voxel image.

### What stage 8 deletes

- `GpuVoxelTileStore`, `GpuVoxelShelf`, `Part_Signature`, `Assign_Tiles`, `GpuVoxel_Fits` and its use in `Pass_Fits`.
- The voxel image (`_Image`, `_Target` in `code/gpumapvoxel.cpp`), `GPU_VOXEL_IMAGE_SIZE` and the
  negative-offset branch for it in `Source_Offset` and `fs_mapdraw.sc`, and `fs_mapvoxel.sc`.
- The tile half of `vs_mapvoxel.sc`, once the splat and faces live in `vs_voxelscene.sc`.
- The `GPU_VOXEL` and `GPU_VOXEL_MASK` comparisons, which read tiles. `Gpu_Voxel_Walk` and `tests/gpuvoxel` stay only
  if we keep a GPU copy of the classic look (a decision below).

## Risks and open questions

- **Draw order and depth.** Slices keep the painter's order between objects, and only direct voxel draws touch the
  depth buffer, so a sprite, a translucent effect or an additive line keeps its place. Voxel blits have no additive
  kind (`BlitDesc::KindType`). The new behaviour is inside one object, which the `Ordered` mode can always turn back.
  A pass with more than 4,096 direct blits takes the image route for those parts until a depth clear with a view split
  lifts the limit; Mental Omega missions are where to count blits first.
- **Fallback still switches look.** A pass `Pass_Fits` refuses for other reasons (more than 8 sonic waves, a map view
  wider or taller than 16,384 pixels, the chronoshift effect, an unsupported draw) is drawn in software and shows the
  classic voxels for that frame. The direct route removes the voxel causes only.
- **Zoomed out at 4K.** At the campaign and skirmish limit (`VIEW_ZOOM_MIN`, half the native zoom, `View_Zoom_Min` in
  `code/viewzoom.cpp`), the map view is twice the screen's map area each way: up to 7,680 by 4,320 map pixels on a
  3840x2160 screen. `Pass_Fits` used to refuse every view over 2^24 pixels, so such passes were drawn in software
  at about 20 ms a picture. The depth and alpha buffers are now addressed by row and column, the limit is gone, and
  on XMP04T8 (a 7546x4245 view) every pass is drawn on the GPU ([GPU_MAP.md](GPU_MAP.md), Limits). What remains at
  that size is a map view wider or taller than 16,384 pixels, and the scale cap described under GPU memory below.
- **Interface and minimap.** The radar draws units as pixels (`RadarTrackingClass`) and the sidebar draws shapes; neither
  reads the map pass, so they do not change. The CPU picture holds no voxel pixels while the GPU draws the pass, as
  with `GpuVoxels=yes` today.
- **Render thread.** All bgfx calls stay on the main thread ([RENDER_THREAD.md](RENDER_THREAD.md)). Face store uploads
  must use `bgfx::copy`, never `bgfx::makeRef`, because the render thread can read them after the frame is handed over,
  the same rule `Upload_Pass_Stores` follows.
- **Replays and determinism.** The direct route reads prepared objects, matrices and blits and writes only render-side
  caches. It must not draw from the sync random generator or write object fields beyond those the draw code writes
  today (`TechnoClass::Draw_Voxel` calls `Calculate_Sinking_Offset`, which sets `SinkingYOffset`). Check:
  `pacing-determinism.txt` and the `--hash` comparison of `scripts/compare_digests.py` identical with the key on and
  off, plus a `syncrec` run.
- **Other bgfx backends.** The map and voxel shaders are embedded for Direct3D 11 only (`BGFX_EMBEDDED_SHADER_D3D11` in
  `code/gpumap.cpp` and `code/gpumapvoxel.cpp`), and both `Start` functions refuse other renderers. The new shaders
  should be compiled for SPIR-V, Metal and GLSL as well and embedded with `BGFX_EMBEDDED_SHADER`, with the depth range
  from `bgfx::getCaps()->homogeneousDepth`. D24S8 is not available on every Metal GPU, so the depth format must be
  picked from what bgfx reports. We do not run non-default renderers in automated runs (a Vulkan run froze a
  development PC), so these checks need a person. The backend stage under Stages covers this work and the Direct3D
  12 measurement.
- **GPU memory at 4K.** At a 3840x2160 map view, one RGBA8 scene-sized texture is 33.2 MB at scale 1, 132.7 MB at
  scale 2 and 530.8 MB at scale 4, and `Size_Target` already makes three (target, snapshot, post): about 100 MB,
  400 MB and 1.6 GB. The direct route adds a D24S8 attachment of the same size and an RG8 attachment of half, about
  1.5 more of these textures, and frees the 8.4 MB voxel image and the 16.8 MB model texture. Scale 4 at 4K is about
  2.4 GB of targets. `Pass_Scale` in `code/gpumap.cpp` lowers the scale of a pass whose scene-sized texture would be
  over 134,217,728 pixels (536.9 MB in RGBA8) or 16,384 pixels a side, about what scale 4 takes at a 3840x2160 view.
  Zoomed fully out at 3840x2160 (7546x4245), scales 3 and 4 draw at 2, 512.5 MB a texture. `GpuVoxelScale=0` draws
  that view at scale 1, 128.1 MB a texture.
- **Small triangles and discard.** Faces at low scale and the fragment discard of the row test are the two costs we
  expect most; the splat level of detail answers the first, measurement the second.
- **Modding.** Custom voxels need nothing new: the face store is built from the loaded layer and keyed by
  `VoxelLibrary::Get_Serial`, so a reloaded library gets new faces. HVA animation changes only the layer matrix a draw
  uploads, not the stored faces. A large Mental Omega map with many distinct voxel types fills the face store; like
  the model store it is emptied and refilled, and a pass whose layers do not fit an empty store is drawn in software.
  The first measurement below tells how close stock and Mental Omega art come.

## Performance budget

Our target is 240 pictures a second, 4.17 ms each, fully zoomed out at 4K on large Mental Omega missions. The baseline is
the Cost section of GPU_VOXELS.md: `bench-large-battle.txt` and `bench-late-game.txt` at 3840x2160 with `GpuRender=yes`,
`RenderFrameRate=240`, HDR Enhanced highlights, `GpuVoxels=yes` and `GpuVoxelSmooth=yes`.

| Measure | Today, large battle, scale 2 | Today, late game, scale 2 | Budget for the direct route (proposal) |
|---|---:|---:|---:|
| Mean draw (main thread) | 1.91 ms | 3.89 ms | Not above today's |
| p95 draw | 3.94 ms | 12.78 ms | Not above today's |
| Draws a second (capped at 240) | 240 | 132 | 240 on the large battle |
| `draw.voxels` | 0.17 ms (scale 1) | | 0.2 ms |
| `draw.gpumap` above `GpuVoxels=no` | about 0.4 ms | 0.4 to 0.7 ms | 0.2 ms, with tiles and signatures gone |
| `present.gputime` | 1.26 ms | | 1.5 ms, voxels included |
| Voxels a pass | 531,000 (scale 1) | | |
| Vertices a pass when every part is drawn | 4.2 million | | 6 a front face, 2 a splat |

With the keys off, the late game's p95 draw is already 11.65 ms on the main thread, almost three times the 4.17 ms a
picture has, so the voxel route alone cannot reach the target there; the direct route must at least not add to it.

What we measure first, in this order:

1. **Faces per layer.** An offline count over every VXL in Yuri's Revenge and Mental Omega: voxels, exposed faces, and
   front faces for each of the eight anchor orientations. Applied to the layers the large battle draws (196 layer
   draws a pass), it gives vertices a pass and the face store size before any shader is written.
2. **The overflow scene.** 48 Kirovs at 3840x2160 at scale 2 and 4, with the stage 1 prototype: `present.gputime`,
   `draw.gpumap` and the count of passes drawn in software, against the image route.
3. **The zoomed-out baseline.** A large battle fully zoomed out at 3840x2160 on a map of 145x137 cells or more, such as
   XMP04T8. `bench-large-battle-zoomed-out.txt` does not measure this: it runs on XMP08T2, where `View_Zoom_Min` keeps
   the zoom at 1.00 at 4K. `bench-large-battle-xmp04-zoomed-out.txt` does, with the numbers below.

**Zoomed out at 4K.** Until the 2^24-pixel limit was lifted, every pass fully zoomed out at 3840x2160 on a large map
was drawn in software. On XMP04T8 at zoom 0.50 (a 7546x4245 view), uncapped (`VSync=no`, `FrameRateLimit=0`) with
`GpuRender=yes`, times are means of the per-second `Perf` lines after the first:

| Run | Passes in software | Mean draw | p95 draw | `present.gputime` | Draws a second |
|---|---:|---:|---:|---:|---:|
| Before, no units | 3,597 of 3,600 | 19.89 ms | 23.62 ms | 0.57 ms | 51 |
| After, no units | 0 of 19,800 | 0.54 ms | 0.82 ms | 0.25 ms | 1,466 |
| After, 4 vehicles, 4 infantry and 2 aircraft, `GpuVoxels=yes`, scale 1 | 0 of 7,800 | 0.86 ms | 1.15 ms | 0.78 ms | 985 |
| The same at `GpuVoxelScale=4`, drawn at 2 | 0 of 7,800 | 1.14 ms | 1.89 ms | 1.02 ms | 771 |

At zoom 0.59 and 0.69 the software draw took 13.52 ms and 9.97 ms before; after, 0.56 ms and 0.47 ms, with no pass
in software.

The large battle is `bench-large-battle-xmp04-zoomed-out.txt`: the forces of `bench-large-battle.txt` (336 objects) on XMP04T8 with the map revealed, at zoom 0.50 and 3840x2160. No pass is drawn in software. Capped at `RenderFrameRate=240` and uncapped, steadier of two runs, with other games running on the machine (`scratch-agent/direct-voxels/ZOOMED_OUT.md` has every run):

| Keys | Capped draws a second | Capped mean / p95 draw | Uncapped draws a second | Uncapped mean / p95 draw | Uncapped `draw.gpumap` | Uncapped `present.gputime` |
|---|---:|---:|---:|---:|---:|---:|
| `GpuVoxels=no` | 299 | 0.62 / 1.52 ms | 533 | 1.38 / 4.04 ms | 0.12 ms | 0.64 ms |
| Image, scale 1 | 269 | 1.71 / 3.94 ms | 286 | 2.95 / 5.11 ms | 0.70 ms | 0.79 ms |
| Image, scale 2 | 240 | 2.02 / 4.19 ms | 298 | 2.86 / 5.86 ms | 0.67 ms | 1.33 ms |
| Direct, scale 1 | 219 | 2.85 / 7.04 ms | 252 | 3.42 / 5.56 ms | 0.74 ms | 1.07 ms |
| Direct, scale 2 | 229 | 2.58 / 6.38 ms | 251 | 3.44 / 5.47 ms | 0.73 ms | 1.84 ms |

`draw.voxels` is 0.12 to 0.14 ms on every voxel route. Direct does not reach 240 draws a second capped, and uncapped it takes 3.97 ms a picture against the 4.17 ms budget with a p95 of 5.5 ms. Of its 3.42 ms draw, the present takes 1.71 ms (`present.upload` 0.80, `present.gpu` 0.90), `draw.gpumap` 0.74 ms and `draw.objects` 0.58 ms; `frame.logic` adds 0.50 ms. "Stage 6 (done)" has a later measurement of the same scene under less load, with the direct route at 309 to 319 draws a second.


## Settings

All keys are in `[Video]` of `RA2MD.INI` unless marked. Current keys keep their meaning.

| Key | Values | Default | Status |
|---|---|---|---|
| `GpuVoxels` | `yes`, `no` | `no` | Unchanged: draws voxels on the GPU, by the route `GpuVoxelPath` picks |
| `GpuVoxelPath` | `Image`, `Direct` | `Direct` | Added in stage 1 as a name list in `code/optionsettings.cpp`; the **Draw voxels directly** switch of the **GPU voxels** group sets it, and is greyed until **Draw units on the GPU** is ticked |
| `GpuVoxelSmooth` | `yes`, `no` | `no` | Unchanged |
| `GpuVoxelScale` | 0 to 4 | `0` | Since stage 7, 0 takes the scale from the zoom and is the default; 1 to 4 are unchanged |
| `GpuVoxelLayerDepth` | `Ordered`, `Shared` | `Shared` | Added in stage 1: a developer key with no row; `Ordered` is the exact mode the checks use |
| `GpuVoxelCloak` | `Translucent`, `Ripple` | `Translucent` | Added after stage 4: a developer key with no row; `Ripple` needs `GpuVoxelPath=Direct` |
| `GpuVoxelSmoothTilt` | `yes`, `no` | `no` | Later |
| `VoxelSceneDepth` in `[AudioVisual]` of `rulesmd.ini`, and per type in `artmd.ini` | `Row`, `Voxel` | `Row` | Later mod key: the row test as now, or the per-voxel footprint depth. A mod sets it because its `ZFudge*` values decide whether `Voxel` looks right |

A pass picks its route like this:

| Condition | Voxel objects are drawn by |
|---|---|
| `GpuRender=no`, the classic interface, or a pass `Pass_Fits` refuses | The software |
| `GpuVoxels=no` | The software voxel renderer, shown as ordinary blits |
| `GpuVoxels=yes`, `GpuVoxelPath=Image` | The image route |
| `GpuVoxels=yes`, `GpuVoxelPath=Direct`, Direct3D 11 with vertex ID support | The direct route for the part kinds the current stage covers, the image route for the rest; after stage 8, the direct route for all parts |

A direct program that fails to build leaves `GpuVoxelPath=Direct` drawing by the image route until stage 8, and by the
software afterwards, with one log line, as `Start` in `code/gpumapvoxel.cpp` does today.

## Decisions

These are provisional until confirmed; each is cheap to reverse before stage 8.

- `Shared` layer depth is the default, so turret and barrel overlap by real depth. `Ordered` stays as the developer key
  the exact checks use.
- Turreted vehicles get highlights under Glow and HDR Enhanced in the GPU look. The software look keeps none, as in
  gamemd.
- `GpuVoxelPath=Direct` is the default for everyone who has not set the key (decided 2026-11-10); `GpuVoxels` keeps its default `no`. The image route stays as `GpuVoxelPath=Image` until stage 8 deletes it, and `gpu-voxel-overflow`, `gpu-voxel-scale`, `gpu-voxel-compare`, `gpu-voxel-shadow` and `gpu-voxel-shadow-bloom` pin it.
- Stage 8 deletes the image route. The software renderer is the only classic reference.
- The 2^24-pixel map view limit is lifted (done before stage 7, after a run on XMP04T8 at 3840x2160 confirmed that the
  zoomed-out view passed it).

## References

- Fatahalian, Boulos, Hegarty, Akeley, Mark, Moreton and Hanrahan, "Reducing Shading on GPUs using Quad-Fragment
  Merging", ACM Transactions on Graphics 29(4), SIGGRAPH 2010: the shading waste of pixel-sized triangles.
  <https://graphics.stanford.edu/papers/fragmerging/>
- Schütz, Kerbl and Wimmer, "Rendering Point Clouds with Compute Shaders and Vertex Order Optimization", Computer
  Graphics Forum 40(4), 2021: compute-shader splatting. <https://arxiv.org/abs/2104.07526>
- Majercik, Crassin, Shirley and McGuire, "A Ray-Box Intersection Algorithm and Efficient Dynamic Voxel Rendering",
  Journal of Computer Graphics Techniques 7(3), 2018: ray-cast voxel boxes. <https://jcgt.org/published/0007/03/04/>
