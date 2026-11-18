# GPU map drawing

The GPU draws the map view's object pass in place of the software blitters. It is on by default
(`GpuRender=yes` in the `[Video]` section of `RA2MD.INI`) and works only in the modern interface on
Direct3D 11; elsewhere the pass is drawn in software. Set `GpuRender=no`, or turn it off on the Graphics
page of the settings, to draw in software everywhere.

## What exists

| Part | File | Role |
|---|---|---|
| Recorder | `code/gpudraw.cpp` | Records each draw of the object pass into the map view as a `GpuBlit`: blits from `RLE_Blit` and `Bit_Blit` (colour indices, depth values, translator, lighting remap and blend kind), solid fills, particle pixels, spot lights, the pixels of plain, depth-shaded, additive and antialiased lines (mind-control links, trails, electric bolts and lasers), and the pixels sonic and magnetron waves shift. |
| GPU renderer | `code/gpumap.cpp` | Draws a recorded pass with the map shaders into a texture that the present shows in place of the software picture. |
| Shaders | `code/shaders/mapdraw/` | `vs_mapdraw.sc`, `fs_mapdraw.sc` (the pass), `fs_mapblur.sc` (soft shadow blur), `fs_mapglow.sc` (bloom glow), `fs_mappost.sc` (shockwaves, bloom, cloud shadows and weather), `fs_mapmask.sc` (the emissive mask of a pass with GPU voxels), `vs_mapclear.sc` with `fs_mapclear.sc` (emptying depth and stencil in the middle of a pass), and `vs_voxelscene.sc` and `vs_voxelface.sc` with `fs_voxelscene.sc` (voxel parts drawn straight into the pass as lines, cube outlines and faces, [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md)), each compiled into a `.bin.h` header. `mapcommon.sh`, `voxelwalk.sh` and `voxelshade.sh` hold the code those shaders share. |
| Voxel renderer | `code/gpumapvoxel.cpp` | Draws the voxel parts of a pass straight into it, at the place of their blits ([GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md)). |

Voxel objects that are blitted straight into the map view can be drawn on the GPU from their voxels with
`GpuVoxels`; [GPU_VOXELS.md](GPU_VOXELS.md) describes it. `RenderThread` moves the GPU's submission and the present to
their own thread; [RENDER_THREAD.md](RENDER_THREAD.md) describes it.

The terrain, buildings and trees that stand still are still drawn in software into the cached
tile layer. Each frame the GPU receives that picture, the depth buffer and, when a blit needs it,
the alpha buffer, then draws the objects over it. When the map view is drawn more than once before
a frame is presented, only the last pass goes to the GPU. With `SoftShadows`, a shadow drawn into the
tile layer marks a mask kept beside the layer instead of darkening its pixels, and the GPU pass
darkens the ground through that mask. With `WaterRipples`, the same mask marks the pixels of water
tiles that no later tile, extra tile image or opaque draw in the layer covers, and the GPU pass
ripples the ground only there, before it draws the objects. A shadow on the water ripples with it.

After 30 passes in a row that the GPU drew, the tile layer is no longer copied into the frame; the
GPU reads it from the tile layer directly. On Windows the GPU keeps a texture of the tile layer's
memory as it lies, and a scroll moves the layer's picture inside that memory without copying it, so
a scroll sends only the strips it brings into view ([Uploads](#uploads)). Elsewhere the layer is
copied on a scroll and compared whole. If a pass then
falls back to software, the draws the recorder does not hold are missing from that one frame, and
the copy resumes until the GPU has again drawn 30 passes in a row.

The autotest steps `gpureplay` and `gpucheck` check the recorder and the GPU against the software
result; [AUTOTEST.md](AUTOTEST.md) documents them.

## Uploads

On Windows the tile layer, the depth buffer and the alpha buffer go to the GPU in parts, and nothing goes
up for a pass that wrote to none of them.

The tile layer's memory is twice the size of the largest picture it is made for, up to 16,384 rows; while
the view is zoomed, that is the largest view the zoom allows ([PERFORMANCE.md](PERFORMANCE.md#zoom-changes)).
A scroll moves the place where the picture starts in that memory (`DSurface::Scroll`), as the depth and alpha
buffers have always done, and the shader finds a pixel of each by row and column from that start. A zoom
change after the first on the same screen moves the start of all three the same way, so only the edges the
new view adds go up. When the picture reaches an end of the memory, the part that stays in view is copied
back to the middle and the whole layer goes up once. `OPENYR_NO_TILE_SCROLL=1` in the environment makes a
scroll copy the layer instead, which is how the two pictures were compared.

The background pass of the map view names each rectangle it draws into (`Tactical::Note_Layer_Writes`):
the whole view on a full redraw, the strips a scroll brings in, the cells flagged for redraw and the dirty
areas. The depth and alpha buffers name what their own pans, fills and updates write. A pass sends the
named rectangles, each once: one named again or lying inside another is left out. A memory page written
outside every named rectangle goes up as whole rows, so a draw that names nothing still reaches the GPU,
unless it shares a page of 4,096 bytes with a named rectangle. More than 512 rectangles named between two
passes count as the whole buffer.

The emissive mask under the map view goes to the GPU with `Bloom`, and with `GpuVoxels` while the mask is kept
([EMISSIVE_MASK.md](EMISSIVE_MASK.md)). A pass compares it with the copy the GPU holds in tiles of 32 by 16 pixels
and sends only the tiles that changed.

A new draw into the tile layer, the depth buffer or the alpha buffer that is not clipped to one of those
rectangles must name its own (`DSurface::Note_Written`, `ZBuffer::Note_Written`, `ABuffer::Note_Written`);
otherwise the GPU can keep showing the old pixels beside a rectangle that was named.

The autotest step `uploadcheck` ([AUTOTEST.md](AUTOTEST.md)) and `OPENYR_UPLOAD_CHECK=1` in the environment
compare each upload with the buffer it came from and log an `UPLOAD_CHECK DIFFERENCE` line for an upload
that left a value out. `tests/autotest/tile-scroll-uploads.txt` scrolls a zoomed-out view in every
direction under the check.

## Textures

A pass makes a texture when it first needs it. The scene target, which the presenter shows, and the post, glow and emissive
mask targets have the size of the pass's area, with a depth attachment of the same size when voxel parts are drawn, because
the presenter finds a pixel by its texture coordinate and a larger texture would round some pixels to other texels. They are
made again when the area changes, so a zoom step makes the scene target of its size. The two soft shadow masks are read by
pixel position, so they are made for the largest area so far or the view `GpuMap_Reserve` names, which the map surfaces
([PERFORMANCE.md](PERFORMANCE.md#zoom-changes)) set to the largest view the zoom limit allows, and a pass uses their corner.
The readback and the snapshot exist only for `gpucheck` and for sonic waves and rippled cloaks. The `gpumemory` autotest step
logs what is held and how many textures have been made ([AUTOTEST.md](AUTOTEST.md)).

## Light fades

A change of the ambient light used to redraw the whole tile layer at every step, because each cell's
brightness goes into the colours the software draws. While the GPU reads the tile layer in place, a step
now leaves the layer alone (`code/layerfade.h`):

- The layer keeps the picture it drew at the light level the fade started from. Anything drawn into the
  layer during the fade, such as a strip a scroll brings in or a building that changes, is drawn with the
  lighting of that level, so the layer stays one consistent picture.
- For each pass the map view gets a plane of scales, one for each block of 4 by 4 pixels: the brightness
  the cell there has now divided by the brightness it had at the frozen level. The shader multiplies the
  base picture by it. Objects drawn over the layer take the current light as before.
- When the light has stayed at its target for 30 game frames the layer is redrawn once at the current
  light, and the picture is exact again. A redraw of the whole layer for any other reason does the same.
- A picture darkened to under half its light waits 450 game frames instead. A dip into darkness that lifts
  again within that time costs no redraw at all: the layer still holds the brighter picture, which is exact
  again when the light is back.
- The layer is also redrawn when the flat ground would have to be brightened to more than 1.5 times the
  frozen picture, since a dark picture scaled far up shows coarse steps. Darkening has no such limit.
- The plane of scales is filled again only when the view, a light level or 15 game frames have passed.

During the fade the terrain is therefore close to, but not exactly, what the software would draw: a cell's
scale is exact for its ground, the scale of a pixel near a cell's edge is blended with its neighbour's, and
colours the software clamps at full brightness scale from the clamped value. A frame the GPU cannot draw ends
the deferral with a full redraw. `OPENYR_NO_FADE_DEFER=1` in the environment turns the deferral off. A `windowshot` step with `asis`
([AUTOTEST.md](AUTOTEST.md)) shows the scaled picture; a plain one redraws the layer first.
`tests/autotest/ambient-fade-deferred.txt` runs two fades.

## Settings

All keys are in the `[Video]` section and are read at startup. The **GPU drawing** switches on the
Graphics page of the settings set `GpuRender`, `SoftShadows`, `Shockwaves`, `Bloom` (the **Glow**
switch), `WaterRipples`, `CloudShadows` and `Weather` when the page is accepted. The **GPU voxels**
group sets `GpuVoxels`, `GpuVoxelSmooth` and `GpuVoxelScale`. The game writes every key in the table
back to `RA2MD.INI`. The effect switches and the voxel rows are greyed out until GPU drawing is
ticked, and GPU drawing is greyed out without the modern interface. Ticking **Weather** sets
`Weather=Map` unless `RA2MD.INI` already chose `Rain` or `Snow`, and clearing it sets `None`.
The rows and keys are declared in `code/optionsettings.cpp`; [SETTINGS.md](SETTINGS.md) describes it.
`SoftShadowRadius`, `BloomStrength`, `WaterRippleStrength`, `CloudShadowStrength` and
`WeatherStrength` have no switch.

| Key | Default | Effect |
|---|---|---|
| `GpuRender` | `yes` | Draws the object pass on the GPU. A pass holding a draw the recorder does not support is drawn in software instead and shows no soft shadows or shockwaves. |
| `SoftShadows` | `no` | With `GpuRender`, the shadows of objects drawn in the pass and the shadows of buildings and trees in the tile layer go into one mask that is blurred and darkens the ground once, so overlapping shadows do not darken twice. A pass drawn in software shows the same shadows hard. Changing the switch redraws the whole tile layer. |
| `SoftShadowRadius` | `3` | The blur radius of `SoftShadows` in pixels, from 1 to 16. |
| `ShadowStacking` | `yes` | `no` puts the shadows of the pass into the same mask without the blur, so overlapping shadows darken the ground once with hard edges; it also applies in software ([GPU_VOXELS.md](GPU_VOXELS.md#stacked-shadows)). |
| `Shockwaves` | `no` | With `GpuRender`, each weapon impact that shows a blast animation sends a ring across the ground that bends the picture under it for 40 game frames. Up to 16 rings run at once; a newer one replaces the oldest. |
| `Bloom` | `no` | With `GpuRender`, the parts of the map marked in the emissive mask, such as lasers, electric bolts, lamps and fire, glow onto the picture around them, about 24 pixels out. It turns the emissive mask on, which [EMISSIVE_MASK.md](EMISSIVE_MASK.md) describes, so an object or animation glows only when the mask marks it. The mask does not change HDR highlights: `HDRHighlights=Classic` still shows classic highlights. A pass drawn in software has no glow. |
| `BloomStrength` | `100` | The brightness of the `Bloom` glow in percent, from 1 to 400. |
| `WaterRipples` | `no` | With `GpuRender`, the water of the map ripples: two slow waves shift the ground under the water by up to 2.4 pixels sideways and 1.2 pixels up or down, and lighten or darken it by up to 10%, and sunlight glints in drifting patches along the crests. Ships, units and effects on the water are drawn after the ripple and do not move with it. Shore tiles that are part land do not ripple, and a pass drawn in software shows still water. Changing the switch redraws the whole tile layer. |
| `WaterRippleStrength` | `100` | The size of the `WaterRipples` shift and shading and the brightness of its glints, in percent, from 1 to 400. |
| `CloudShadows` | `no` | With `GpuRender`, the shadows of clouds a few hundred pixels across drift slowly over the map, about 10 pixels a second, and darken everything the pass draws under them, units included, by up to 30%. They cover about a third of the map and stay fixed to the ground as the view scrolls. A pass drawn in software shows no clouds. |
| `CloudShadowStrength` | `100` | How much a cloud darkens the map in percent, from 1 to 400; 400 darkens it completely. |
| `Weather` | `None` | With `GpuRender`, `Map` makes rain fall over the whole map while a lightning storm rages and otherwise makes snow fall on maps whose theater sets `IsArctic=yes`, which in Yuri's Revenge is the snow theater; other maps show no weather. `Snow` and `Rain` fall on every map; rain also dims the picture, by 6% at the usual amount and at most 12%. Under rain, each lightning storm bolt brightens the picture for 8 game frames, at most once every 45 game frames. The flakes and drops are drawn over everything in the pass, fall in two or three layers at different speeds, and move with the ground as the view scrolls. A pass drawn in software shows no weather. |
| `WeatherStrength` | `100` | How much snow or rain falls, in percent, from 1 to 400. |
| `GpuVoxels` | `no` | With `GpuRender`, vehicles, aircraft, their shadows, voxel animations and bullets are drawn from their voxels on the GPU, and vehicles and aircraft with no turret feed their highlights to `Bloom` and HDR Enhanced. The Graphics page sets it. See [GPU_VOXELS.md](GPU_VOXELS.md). |
| `GpuVoxelSmooth` | `no` | With `GpuVoxels`, an object the GPU draws from its voxels turns to its exact facing instead of the nearest of 32. See [GPU_VOXELS.md](GPU_VOXELS.md). |
| `GpuVoxelScale` | `0` | With `GpuVoxels`, 1 to 4: draws the map pass at that many times the map's pixels, with each voxel as its cube, so voxel objects stay sharp in a zoomed map view. 0, shown as **Auto**, takes the scale from the zoom: the screen pixels a map pixel covers, rounded to the nearest whole number from 1 to 4. A very large map view draws at a lower scale (Limits). See [GPU_VOXELS.md](GPU_VOXELS.md). |

## Limits

- The shaders exist for Direct3D 11 only. On other renderers `GpuRender` draws nothing on the GPU,
  and `gpucheck` logs one `GPU_DIFF unavailable` line.
- Copies through a translator match the software picture exactly. Darken and the 25%, 50% and 75%
  translucent kinds finish in the hardware blend in 8-bit colour. The software blends floor each
  halved or quartered term; the shader floors the source term and offsets the target term by its
  average flooring loss. In a 1,500-frame large battle at 1920x1080 this left about 180 pixels per
  pass differing from the software replay, almost all by one step in a 565 channel; the largest
  difference was 4 steps.
- Spot lights brighten in 8-bit colour on the GPU, so they can differ from the software result by
  one step.
- Lines are tested against the depth buffer when they are recorded, so their pixels match the
  software line. A fading trail and an antialiased laser blend in the hardware blend without the
  software's rounding. An antialiased laser blends each pixel from its own colour, while the software
  blends some pixels from their neighbour's. An additive laser glow that saturates a channel stops at
  white in that channel, while the software hands the excess to the other channels.
- A sonic or magnetron wave lifts each shifted pixel from the picture as it stood before the wave.
  The software lifts some pixels from ones the wave has already shifted, so the ripple can differ by
  a pixel. A pass with more than 8 waves is drawn in software.
- A pass that would not fit the GPU's limits is drawn in software: more than 8 waves, a map view, depth buffer or alpha
  buffer wider or taller than 16,384 pixels, an arena past the texture size, too many vertices for the frame, or more
  than 12,582,912 bytes of blit pixels outside the shape atlas or 16,777,216 bytes of depth shapes, whose offsets the
  shader reads as 32-bit floats. If the GPU still cannot take the kept pass when the frame is presented, that frame
  shows the last GPU picture and the next pass is drawn in software.
- The map view's pixel count has no limit of its own, because the shader finds a pixel of the depth and alpha buffers
  by row and column. Zoomed fully out on a 3840x2160 screen, a 7546x4245 view (32 million pixels) is drawn on the GPU
  (times in [GPU_DIRECT_VOXELS.md](GPU_DIRECT_VOXELS.md), Performance budget), and `tests/autotest/gpu-map-zoomed-out.txt`
  checks that no such pass is drawn in software.
- At a voxel scale above 1, a pass whose picture at that scale would be wider or taller than 16,384 pixels or
  larger than 134,217,728 pixels (536.9 MB in RGBA8, a little over a 3840x2160 view at scale 4) is drawn at the
  largest scale within both limits, down to 1. Zoomed fully out at 3840x2160, scales 3 and 4 draw at 2.
- While the chronoshift screen effect runs, the pass is drawn in software, because the effect swirls a
  picture of the finished map view.
- At 3840x2160 the first 1,500 frames of the large battle benchmark drew 254 pictures per second
  with `GpuRender`, which is the benchmark's limit of 240 per second plus extra draws, and 175 in
  software. With `SoftShadows` it drew 243, with `Bloom` 255, with `WaterRipples` 250, with
  `CloudShadows` and `Weather=Snow` 244, and with every effect on 230; no effect added more than half a millisecond of processor time to a picture. Each pass uploads only
  the parts of the base picture, the depth buffer and the alpha buffer that changed since the last
  drawn pass ([Uploads](#uploads)). Outside Windows the depth and alpha buffers cannot list the pages written to them,
  so those two go up whole every pass. With the frame rate uncapped (`FrameRateLimit=0`, `VSync=no`)
  the same benchmark drew about 507 pictures per second with `GpuRender` and 154 in software.
- On Windows the game watches the frame surface's written pages. The frame is split into 32 column
  blocks, and each row records the blocks written. A GPU pass refills the map layer key only in the
  blocks written since the last fill. Every picture copies to the visible surface only the blocks written
  since the last copy, and the presenter compares only the rows that hold them. The whole frame is copied
  every 120th picture, while the screen shakes or is zoomed, and after anything else has changed the
  visible surface. In alternating runs of the
  4K benchmark on a busy machine, this raised the rate from about 119 to 140 pictures per second.
- Cached voxel images, such as vehicle shadows and the hulls of vehicles without a turret, are decoded
  once into an atlas of up to 4 MB that the GPU keeps, and later passes refer to it instead of copying
  their pixels. The voxel images leave the atlas when the cached voxel images are cleared, and the atlas is emptied when it fills. In four
  alternating pairs of the 4K benchmark it raised the rate from about 486 to about 508 pictures per second.
- Frames of shapes, such as infantry, animations and buildings, join the same atlas, including frames drawn with
  a depth shape. A frame is found by the address of its data. The atlas keeps a copy of the frame's compressed rows and
  compares them once per pass, so a frame whose memory was freed and reused for another shape is decoded again. The
  benchmark's atlas held about 400 images in 1.3 MB without filling. It cut the objects phase of a draw by about 0.07
  ms at 4K, from about 0.38 ms to 0.31 ms, and the picture rate stayed within the run-to-run spread.
- The modern build panel's background stays translucent at `SidebarOpacity` over the GPU map. Where the
  frame holds the map layer key, the panel writes one of two tint keys, and the presenter blends the
  panel's fill or well colour over the map there. The result is within 8 of 255 per channel of the
  software picture.

## Rebuilding the shaders

The `.bin.h` files are checked in, so a normal build does not need `shaderc`. After editing a `.sc`
file, rebuild its header from the repository root with a `shaderc` built from
`thirdparty/bgfx.cmake`, for example:

```bash
shaderc -f code/shaders/mapdraw/fs_mapdraw.sc -o fs_mapdraw.bin --type fragment --platform windows -p s_5_0 -O 3 --varyingdef code/shaders/mapdraw/varying.def.sc -i thirdparty/bgfx.cmake/bgfx/src
```

Use `--type vertex` for the `vs_` files. After editing `mapcommon.sh`, `voxelwalk.sh` or `voxelshade.sh` (which `voxelwalk.sh` includes), rebuild every shader that
includes it. Then write each `.bin` into its header as a
`static const uint8_t <name>_dxbc[]` array, keeping the comment line at the top. `shaderc` rejects
the `vs_5_0` and `ps_5_0` profile names; use `s_5_0` for both.

A fragment shader must declare the same `$input` list as its vertex shader outputs (`vs_mapdraw.sc` for most of them), even inputs
it does not read. bgfx refuses to build a program whose fragment inputs differ, and the effect that
uses it then draws nothing.
