# GPU map drawing

The GPU can draw the map view's object pass in place of the software blitters. Set
`GpuRender=yes` in the `[Video]` section of `RA2MD.INI` to turn it on. It is experimental, off by
default, and works only in the modern interface on Direct3D 11.

## What exists

| Part | File | Role |
|---|---|---|
| Recorder | `code/gpudraw.cpp` | Records each draw of the object pass into the map view as a `GpuBlit`: blits from `RLE_Blit` and `Bit_Blit` (colour indices, depth values, translator, lighting remap and blend kind), solid fills, particle pixels, spot lights, the pixels of plain, depth-shaded, additive and antialiased lines (mind-control links, trails, electric bolts and lasers), and the pixels sonic and magnetron waves shift. |
| GPU renderer | `code/gpumap.cpp` | Draws a recorded pass with the map shaders into a texture that the present shows in place of the software picture. |
| Shaders | `code/shaders/mapdraw/` | `vs_mapdraw.sc`, `fs_mapdraw.sc` (the pass), `fs_mapblur.sc` (soft shadow blur), `fs_mapglow.sc` (bloom glow) and `fs_mappost.sc` (shockwaves, bloom, cloud shadows and weather), each compiled into a `.bin.h` header. |

The terrain, buildings and trees that stand still are still drawn in software into the cached
tile layer. Each frame the GPU receives that picture, the depth buffer and, when a blit needs it,
the alpha buffer, then draws the objects over it. When the map view is drawn more than once before
a frame is presented, only the last pass goes to the GPU. With `SoftShadows`, a shadow drawn into the
tile layer marks a mask kept beside the layer instead of darkening its pixels, and the GPU pass
darkens the ground through that mask. With `WaterRipples`, the same mask marks the pixels of water
tiles that no later tile, extra tile image or opaque draw in the layer covers, and the GPU pass
ripples the ground only there, before it draws the objects. A shadow on the water ripples with it.

After 30 passes in a row that the GPU drew, the tile layer is no longer copied into the frame; the
GPU reads it from the tile layer directly, and only when that layer has changed. On Windows the
tile layer lists the memory pages written to it, so a changed layer is compared only on the rows
written since the last pass; elsewhere the whole layer is compared. If a pass then
falls back to software, the draws the recorder does not hold are missing from that one frame, and
the copy resumes until the GPU has again drawn 30 passes in a row.

The autotest steps `gpureplay` and `gpucheck` check the recorder and the GPU against the software
result; [AUTOTEST.md](AUTOTEST.md) documents them.

## Settings

All keys are in the `[Video]` section and are read at startup. The **GPU drawing** switches on the
Graphics page of the settings set `GpuRender`, `SoftShadows`, `Shockwaves`, `Bloom` (the **Glow**
switch), `WaterRipples`, `CloudShadows` and `Weather` when the page is accepted, and the game writes
every key in the table back to `RA2MD.INI`. The effect switches are greyed out until GPU drawing is
ticked, and GPU drawing is greyed out without the modern interface. Ticking **Weather** sets
`Weather=Map` unless `RA2MD.INI` already chose `Rain` or `Snow`, and clearing it sets `None`.
`SoftShadowRadius`, `BloomStrength`, `WaterRippleStrength`, `CloudShadowStrength` and
`WeatherStrength` have no switch.

| Key | Default | Effect |
|---|---|---|
| `GpuRender` | `no` | Draws the object pass on the GPU. A pass holding a draw the recorder does not support is drawn in software instead and shows no soft shadows or shockwaves. |
| `SoftShadows` | `no` | With `GpuRender`, the shadows of objects drawn in the pass and the shadows of buildings and trees in the tile layer go into one mask that is blurred and darkens the ground once, so overlapping shadows do not darken twice. A pass drawn in software shows the same shadows hard. Changing the switch redraws the whole tile layer. |
| `SoftShadowRadius` | `3` | The blur radius of `SoftShadows` in pixels, from 1 to 16. |
| `Shockwaves` | `no` | With `GpuRender`, each weapon impact that shows a blast animation sends a ring across the ground that bends the picture under it for 40 game frames. Up to 16 rings run at once; a newer one replaces the oldest. |
| `Bloom` | `no` | With `GpuRender`, the parts of the map marked in the emissive mask, such as lasers, electric bolts, lamps and fire, glow onto the picture around them, about 24 pixels out. It turns the emissive mask on, which [EMISSIVE_MASK.md](EMISSIVE_MASK.md) describes, so an object or animation glows only when the mask marks it. The mask does not change HDR highlights: `HDRHighlights=Classic` still shows classic highlights. A pass drawn in software has no glow. |
| `BloomStrength` | `100` | The brightness of the `Bloom` glow in percent, from 1 to 400. |
| `WaterRipples` | `no` | With `GpuRender`, the water of the map ripples: two slow waves shift the ground under the water by up to 2.4 pixels sideways and 1.2 pixels up or down, and lighten or darken it by up to 10%, and sunlight glints in drifting patches along the crests. Ships, units and effects on the water are drawn after the ripple and do not move with it. Shore tiles that are part land do not ripple, and a pass drawn in software shows still water. Changing the switch redraws the whole tile layer. |
| `WaterRippleStrength` | `100` | The size of the `WaterRipples` shift and shading and the brightness of its glints, in percent, from 1 to 400. |
| `CloudShadows` | `no` | With `GpuRender`, the shadows of clouds a few hundred pixels across drift slowly over the map, about 10 pixels a second, and darken everything the pass draws under them, units included, by up to 30%. They cover about a third of the map and stay fixed to the ground as the view scrolls. A pass drawn in software shows no clouds. |
| `CloudShadowStrength` | `100` | How much a cloud darkens the map in percent, from 1 to 400; 400 darkens it completely. |
| `Weather` | `None` | With `GpuRender`, `Map` makes rain fall over the whole map while a lightning storm rages and otherwise makes snow fall on maps whose theater sets `IsArctic=yes`, which in Yuri's Revenge is the snow theater; other maps show no weather. `Snow` and `Rain` fall on every map; rain also dims the picture, by 6% at the usual amount and at most 12%. Under rain, each lightning storm bolt brightens the picture for 8 game frames, at most once every 45 game frames. The flakes and drops are drawn over everything in the pass, fall in two or three layers at different speeds, and move with the ground as the view scrolls. A pass drawn in software shows no weather. |
| `WeatherStrength` | `100` | How much snow or rain falls, in percent, from 1 to 400. |

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
- A pass that would not fit the GPU's limits (more than 8 waves, an area or arena past the texture
  size, too many vertices for the frame) is drawn in software. If the GPU still cannot take the kept
  pass when the frame is presented, that frame shows the last GPU picture and the next pass is drawn
  in software.
- While the chronoshift screen effect runs, the pass is drawn in software, because the effect swirls a
  picture of the finished map view.
- At 3840x2160 the first 1,500 frames of the large battle benchmark drew 254 pictures per second
  with `GpuRender`, which is the benchmark's limit of 240 per second plus extra draws, and 175 in
  software. With `SoftShadows` it drew 243, with `Bloom` 255, with `WaterRipples` 250, with
  `CloudShadows` and `Weather=Snow` 244, and with every effect on 230; no effect added more than half a millisecond of processor time to a picture. Each pass uploads only
  the rows of the base picture, the depth buffer and the alpha buffer that changed since the last
  drawn pass. Outside Windows the depth and alpha buffers cannot list the pages written to them,
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
  their pixels. The atlas is emptied when the cached voxel images are cleared or it fills. In four
  alternating pairs of the 4K benchmark it raised the rate from about 486 to about 508 pictures per second.

## Rebuilding the shaders

The `.bin.h` files are checked in, so a normal build does not need `shaderc`. After editing a `.sc`
file, rebuild its header from the repository root with a `shaderc` built from
`thirdparty/bgfx.cmake`, for example:

```bash
shaderc -f code/shaders/mapdraw/fs_mapdraw.sc -o fs_mapdraw.bin --type fragment --platform windows -p s_5_0 -O 3 --varyingdef code/shaders/mapdraw/varying.def.sc -i thirdparty/bgfx.cmake/bgfx/src
```

Use `--type vertex` for `vs_mapdraw.sc`. Then write each `.bin` into its header as a
`static const uint8_t <name>_dxbc[]` array, keeping the comment line at the top. `shaderc` rejects
the `vs_5_0` and `ps_5_0` profile names; use `s_5_0` for both.

Every fragment shader must declare the same `$input` list as `vs_mapdraw.sc` outputs, even inputs
it does not read. bgfx refuses to build a program whose fragment inputs differ, and the effect that
uses it then draws nothing.
