# Drawing voxels straight into the GPU scene (design)

We propose to draw each voxel object as geometry into the map pass's scene target, at the place of its blit in the
pass, with a depth and stencil buffer for the voxels' own depth. The voxel image, its tiles and its overflow go away,
sharpness becomes the resolution of the scene target, and hull, turret and barrel hide each other by real depth.
Nothing here is built yet; every "we propose" below is a design choice, and every statement about current behaviour
cites the code. [GPU_VOXELS.md](GPU_VOXELS.md) owns the current route and [GPU_MAP.md](GPU_MAP.md) the map pass.

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
| Voxel parts | `GpuVoxel_Submit` in `code/gpumapvoxel.cpp`, `vs_mapvoxel.sc` | Each part not already in the 2048 by 2048 RG8 voxel image gets a tile of its buffer region times `GpuVoxelScale`. Its layers are drawn into the tile in the software's walk order: a 2-vertex line a voxel at scale 1, an 8-vertex outline strip above. |
| Voxel blits | `Source_Offset`, `fs_mapdraw.sc` | An offset of `GPU_VOXEL_BASE` and up reads the tile, then runs the translator, alpha lighting, row depth test and blend as for any blit. |
| Model store | `Load_Models`, `Model_Key` | A layer's walk is stored once for each of the eight anchor orientations it is drawn in. |
| Highlights | `Submit_Voxel_Mask`, `fs_mapmask.sc` | A direct part's blit leaves `GPU_VOXEL_MASK_MARK` in the CPU plane; the mask pass replaces the mark with the strength in the part's tile. |

The image route has four limits that this design removes.

- **Overflow.** `GpuVoxel_Fits` and `Assign_Tiles` refuse a pass whose tiles do not fit an empty image, and the pass is
  then drawn in software (`Submit_Pass`, `Pass_Fits`). A tile's side grows with the scale, so its area grows with the
  scale squared. In our measurement with 48 Kirovs at 4K, 595 of 600 passes fell back, and the picture switched
  between the GPU look and the software look. A stop-gap for this is on branch `gpu-voxel-overflow`.
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

Each direct part gets its own depth slice. The n-th direct part in blit order writes depth
`((SLICES - 1 - n) + d) / SLICES`, where `d` in [0, 1) places the voxel inside the part (see the modes below), and
tests with `LEQUAL`, which gives a tie to the later voxel as the walk does. A later part therefore always passes over
an earlier one, which keeps the painter's order between objects, and inside a part `d` decides. With 24 bits and
4,096 slices, each slice has 4,096 depth steps. A pass with more than 4,096 direct parts needs a depth clear, which
means a new view; that is an open question.

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
(`Precalculate_Normal_Lookup`, `code/voxlib.cpp`, uploaded as `u_vlight`) and the row and colour into a palette index
through `VoxelPaletteTranslateTable` (`s_vpal`, `Upload_Palette`). The fragment shader then does what `fs_mapdraw.sc`
does for a copy blit: adds the alpha lighting band from `s_alut` at the blit's level, reads the 565 colour from the
blit's translator row in `s_lut`, which carries the house colours, and writes it. A composite reads the second block
of `s_vpal`, the scratch surface's translation, as now.

We propose to move the depth test, the translator lookup and `Rgb565` from `fs_mapdraw.sc` into a shared include
(`mapcommon.sh`) that both fragment shaders use, so the two cannot drift.

In stage 6 the lighting row is computed on the GPU from the normal tables and the light vector, so `u_vlight` and
the CPU half of `GpuDraw_Capture_Prepared` lose their per-object table. The rows stay palette rows. A float lighting
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

The cloak ripple is an option (proposal, `GpuVoxelCloak=Ripple`): a snapshot of the scene before the first cloaked part,
taken the way a sonic wave takes one (`bgfx::blit` into `_Snapshot` in a new view), and each cloaked fragment reads it
at a small offset. It uses one of the eight wave views, and a cloaked object would not show objects drawn between the
snapshot and itself.

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
mask and do not change. A composite writes no mask today because the scratch surface has none; we propose that a direct
composite writes one, so turreted vehicles glow too. That is a deliberate change from the software.

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
no tile in between. We propose `GpuVoxelScale=0` (stage 7) to mean "from the zoom": the smallest whole scale at which
one map pixel covers at least one screen pixel, capped by the memory budget below.

## What changes for the player

| Look or cost | Classic (software, or `GpuVoxels=no`) | Image route today | Direct route |
|---|---|---|---|
| Picture at scale 1 | gamemd's | Index-exact with the software | Equal with `GpuVoxelLayerDepth=Ordered`; with `Shared`, layers and turret pieces hide each other by depth |
| Large scale with many large units | Unchanged | Falls back to software in most passes and switches look | No voxel image to overflow; the remaining limit is the face store, about 4 million faces at the proposed size |
| Hull, turret and barrel | Order by turret facing | Same | Depth decides |
| Cloaked unit | Ripple from the cached picture (`RLE_Blit` predator offset) | Plain translucent | Plain translucent, ripple as an option |
| Turreted vehicle under Glow or HDR Enhanced | No highlight | No highlight | Highlights (proposal) |
| Turning and tilt | 32 facings | Exact facing with `GpuVoxelSmooth` | Same; smooth tilt as a later option |
| Main thread | Software voxel drawing | Tile hashing, shelf placement, part signatures | No tiles or signatures; per-layer uniforms only |
| GPU | None | Draws only parts not in the image (5% to 37% of parts a second in the Cost runs of GPU_VOXELS.md) | Draws every visible voxel every pass; we expect it to cost more at scale 1 and less where tiles were redrawn often |

The classic look keeps everything gamemd shows, including the cloak ripple from cached pictures, and stays the reference
for `gpucheck` and `GPU_VOXEL`.

## Stages

Each stage has a check that fails when its result breaks. Until stage 8 a part the current stage does not handle takes
the image route in the same pass, so no stage adds a software fallback.

| Stage | What it adds | Check | Shares | Deletes |
|---|---|---|---|---|
| 1 | Direct opaque parts drawn into the scene: vehicles without a turret, aircraft, voxel animations, debris, bullets. D24S8 attachment, depth slices with `Ordered` and `Shared` layer modes, row depth test from the blit, translator and lighting band in `fs_voxelscene.sc`, `GpuVoxelPath=Direct`. While the emissive mask is on, these parts stay on the image route | `gpu-voxel-compare.txt` under `gpucheck` with `GpuRender=no`, `GpuVoxelPath=Direct`, `GpuVoxelLayerDepth=Ordered` at scale 1: no `GPU_DIFF` pixel on a vehicle, as in the image-route run (about three one-step pixels a pass, none on a vehicle). A new scene with 48 Kirovs at `GpuVoxelScale=4` and 3840x2160: no "the voxel parts cannot be drawn" line. A `gpuvoxel` unit test: slice n+1 is nearer than slice n for every `d` | Recorder, prepared-object cache, model store and walk, `Setup_Object` steps, palette texture, translator and lighting stacks, row depth code (moved to `mapcommon.sh`) | Nothing yet |
| 2 | Composites: hull, turret and barrel in one slice, pieces placed from the blit | `gpu-voxel-compare.txt` turret section with `Ordered`: no `GPU_DIFF` pixel on a vehicle. With `Shared`: a `gpuvoxel` unit test that places two pieces at known offsets and checks which one is nearer, and a `gpu-voxel-shot.txt` still of turreted vehicles at turret facings that differ from the body's, compared by eye | Piece placement arithmetic of `GpuVoxel_Submit` | The barrel-before-turret ordering is no longer needed on this route |
| 3 | Shadows as flat cells, stencil once per blit, soft shadows into the mask | `gpu-voxel-shadow.txt` and `gpu-voxel-shadow-bloom.txt`: darkened counts within 5% of 2,098 and 1,970; a new `twice` count in `GPU_VOXEL_SHADOW` of pixels darkened twice by one blit is 0 | `Gpu_Voxel_Walk_Shadow`, `Setup_Shadow`, `Submit_Shadows` | |
| 4 | Translucent, warp and cloak parts with the depth pre-pass; `GpuVoxelCloak` | `cloak-tank.txt` and `chrono-warp-colour.txt` with `Ordered`: `GPU_DIFF` largest step not above the image route's (2 and 1) and mean within its run-to-run spread (161 and 498 in GPU_VOXELS.md) | `Blend_State_For` | |
| 5 | Highlights through the RG8 attachment; parts stay direct while the mask is on | `GPU_MASK` at scale 1 under `gpucheck` with `EmissiveDebugView=yes` and `Ordered`: 0 differing on `gpu-voxel-compare.txt` and `hdr-emissive-scene.txt` with voxel vehicles | `Submit_Voxel_Mask`, `fs_mapmask.sc` structure | `fs_mapmask.sc` reading the voxel image |
| 6 | Face store per layer (no orientation copies), exposed faces only, three visible direction ranges a draw, lighting rows on the GPU, optional face shading | `gpuvoxel` unit test: a single voxel has 6 faces, a solid 3x3x3 block 54, and no listed face has a filled neighbour; the GPU lighting row equals `Precalculate_Normal_Lookup`'s for every normal of the four normal tables under a set of lights. `gpu-voxel-scale.txt` edge counts at scale 2 and 4 not below stage 1's | Face store replaces the model store's slot management (`Load_Models`) | Orientation copies; the per-object `u_vlight` table |
| 7 | Splat below a size threshold, faces above; `GpuVoxelScale=0` from the zoom | `bench-large-battle.txt` and `bench-large-battle-zoomed-out.txt` at 3840x2160 meet the budget below; edge counts at zoom 1, 2, 3 | | |
| 8 | Direct by default, then removal of the image route | The regress suite with `GpuVoxelPath=Direct`; `pacing-determinism.txt` hashes identical with the key on and off | | See below |

Later, outside these stages: smooth tilt, the `Voxel` scene depth mode, and GPU terrain. GPU terrain would draw the
tile layer from tile images and their depth data into the same depth attachment in the map's row units, replacing the
uploaded base picture and `s_zbuf`. The voxel row test would then become a hardware test only if voxels wrote row depth,
which conflicts with the slices; that choice belongs to the terrain design.

### Stage 1 in detail

1. **Key.** Add `GpuVoxelPath` to `code/optionsettings.cpp` with no row, an `OptionsClass` field, and pass it on beside
   `GpuDraw_Set_Voxels`. Add a manual change record and key page as for any new key ([SETTINGS.md](SETTINGS.md)).
2. **Routing.** Before `Pass_Fits` runs, mark each part direct when the key is `Direct`, the part is not a composite,
   every blit that shows it has the copy kind (`BlitDesc::XLAT`), and the emissive mask is off. `GpuVoxel_Fits` (from
   `Pass_Fits` and `Submit_Pass`) and `GpuVoxel_Submit` then skip direct parts, so they take no tile. The part list itself (`GpuDrawList::Voxels`) does not change.
3. **Target.** `Size_Target` creates a D24S8 texture of the target's size and builds `_Target` from both textures.
   `Submit_Draws` clears depth to 1 and stencil to 0 on `VIEW_MAPDRAW` when the pass has a direct part, and the wave
   views keep `BGFX_CLEAR_NONE`.
4. **Model data.** For each direct part, build the `LayerDraw` list as `GpuVoxel_Submit` does and call `Load_Models`, so
   the walks are in the model texture. Add to each layer its depth origin and steps from `BoxCorner`, and, for
   `Ordered`, its sub-slice and voxel count.
5. **Draws.** In the blit loop of `Submit_Draws`, a blit whose part is direct submits one draw per layer with
   `vs_voxelscene.sc` and `fs_voxelscene.sc`: the same model and palette textures and `u_vox` and `u_vlight` as
   `vs_mapvoxel.sc` reads, `s_lut`, `s_alut`, `s_zbuf` and `s_abuf` as `fs_mapdraw.sc` reads, and new uniforms for the
   part's map origin and scale, the slice, the depth steps, and the blit's rectangle, row depth, translator row and
   lighting band row. State: write colour and depth, depth test `LEQUAL`, lines at scale 1 and triangle strips above,
   scissor to the blit's rectangle times the scale.
6. **Shaders.** `vs_voxelscene.sc` is `vs_mapvoxel.sc` with the tile origin replaced by the part's origin and a depth
   output. `fs_voxelscene.sc` discards index 0, runs the row test, and writes the translated 565 colour. Move the shared
   parts of `fs_mapdraw.sc` into `mapcommon.sh` and add any new varyings to `varying.def.sc`. Rebuild the `.bin.h`
   headers as [GPU_MAP.md](GPU_MAP.md) describes.
7. **Checks.** The `gpuvoxel` slice test, the `gpu-voxel-compare.txt` run and the 48-Kirov scene from the table, plus
   `gpu-voxel-scale.txt` at scale 2 and 4, compared with the image route by eye and by edge counts.

### What stage 8 deletes

- `GpuVoxelTileStore`, `GpuVoxelShelf`, `Part_Signature`, `Assign_Tiles`, `GpuVoxel_Fits` and its use in `Pass_Fits`.
- The voxel image (`_Image`, `_Target` in `code/gpumapvoxel.cpp`), `GPU_VOXEL_IMAGE_SIZE`, `GPU_VOXEL_BASE` and the
  branch for it in `Source_Offset` and `fs_mapdraw.sc`, and `fs_mapvoxel.sc`.
- The tile half of `vs_mapvoxel.sc`, once the splat and faces live in `vs_voxelscene.sc`.
- The `GPU_VOXEL` and `GPU_VOXEL_MASK` comparisons, which read tiles. `Gpu_Voxel_Walk` and `tests/gpuvoxel` stay only
  if we keep a GPU copy of the classic look (a decision below).

## Risks and open questions

- **Draw order and depth.** Slices keep the painter's order between objects, and only direct voxel draws touch the
  depth buffer, so a sprite, a translucent effect or an additive line keeps its place. Voxel blits have no additive
  kind (`BlitDesc::KindType`). The new behaviour is inside one object, which the `Ordered` mode can always turn back.
  A pass with more than 4,096 direct parts needs a depth clear, which needs a view split; Mental Omega missions are
  where to count parts first.
- **Fallback still switches look.** A pass `Pass_Fits` refuses for other reasons (more than 8 sonic waves, a map view
  over 16,777,216 pixels, the chronoshift effect, an unsupported draw) is drawn in software and shows the classic voxels
  for that frame. The direct route removes the voxel causes only.
- **Zoomed out at 4K.** At the campaign and skirmish limit (`VIEW_ZOOM_MIN`, half the native zoom, `View_Zoom_Min` in
  `code/viewzoom.cpp`), the map view is twice the screen's map area each way: up to 7,680 by 4,320 map pixels on a
  3840x2160 screen, over the 2^24-pixel limit in `Pass_Fits`. Such a pass is drawn in software today, whatever the
  voxel route. Lifting the limit is GPU_MAP work and comes before the 240 fps
  target can be met.
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
  development PC), so these checks need a person.
- **GPU memory at 4K.** At a 3840x2160 map view, one RGBA8 scene-sized texture is 33.2 MB at scale 1, 132.7 MB at
  scale 2 and 530.8 MB at scale 4, and `Size_Target` already makes three (target, snapshot, post): about 100 MB,
  400 MB and 1.6 GB. The direct route adds a D24S8 attachment of the same size and an RG8 attachment of half, about
  1.5 more of these textures, and frees the 8.4 MB voxel image and the 16.8 MB model texture. Scale 4 at 4K is about
  2.4 GB of targets; `GpuVoxelScale=0` should cap the scale by a memory budget.
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
3. **The zoomed-out baseline.** `bench-large-battle-zoomed-out.txt` at 3840x2160, whose passes the pixel limit keeps in
   software when the map view is over 2^24 pixels, to know how much of the 4.17 ms the software draw already takes.

## Settings

All keys are in `[Video]` of `RA2MD.INI` unless marked. Current keys keep their meaning.

| Key | Values | Default | Status |
|---|---|---|---|
| `GpuVoxels` | `yes`, `no` | `no` | Unchanged: draws voxels on the GPU, by the route `GpuVoxelPath` picks |
| `GpuVoxelPath` | `Image`, `Direct` | `Image`, then `Direct` at stage 8 | New (proposal). A name list (`.Names`, as `Weather` has) in `code/optionsettings.cpp`; no row until stage 5, then a row in the **GPU voxels** group |
| `GpuVoxelSmooth` | `yes`, `no` | `no` | Unchanged |
| `GpuVoxelScale` | 1 to 4; 0 from stage 7 | `1` | Unchanged for 1 to 4; 0 picks the scale from the zoom |
| `GpuVoxelLayerDepth` | `Ordered`, `Shared` | `Shared` | New developer key with no row; `Ordered` is the exact mode the checks use |
| `GpuVoxelCloak` | `Translucent`, `Ripple` | `Translucent` | New (proposal), stage 4 |
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

## Decisions we need

- Whether `Shared` layer depth is the default, given that it changes how turret and barrel overlap compared with gamemd.
- Whether turreted vehicles get highlights under Glow and HDR Enhanced, which the software never shows.
- Whether to keep a GPU copy of the classic look (the scale 1 image route and the `GPU_VOXEL` comparison) after stage 8,
  or delete it and leave the software as the only classic reference.
- Whether lifting the 2^24-pixel map view limit is scheduled before stage 7, since the zoomed-out 4K target depends on it.

## References

- Fatahalian, Boulos, Hegarty, Akeley, Mark, Moreton and Hanrahan, "Reducing Shading on GPUs using Quad-Fragment
  Merging", ACM Transactions on Graphics 29(4), SIGGRAPH 2010: the shading waste of pixel-sized triangles.
  <https://graphics.stanford.edu/papers/fragmerging/>
- Schütz, Kerbl and Wimmer, "Rendering Point Clouds with Compute Shaders and Vertex Order Optimization", Computer
  Graphics Forum 40(4), 2021: compute-shader splatting. <https://arxiv.org/abs/2104.07526>
- Majercik, Crassin, Shirley and McGuire, "A Ray-Box Intersection Algorithm and Efficient Dynamic Voxel Rendering",
  Journal of Computer Graphics Techniques 7(3), 2018: ray-cast voxel boxes. <https://jcgt.org/published/0007/03/04/>
