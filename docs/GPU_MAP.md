# GPU map drawing

We are moving the map view's object pass from the software blitters to the GPU. The work is in
progress: today the GPU draws a copy of each pass for comparison, and the game still shows the
software picture.

## What exists

| Part | File | Role |
|---|---|---|
| Recorder | `code/gpudraw.cpp` | Records each blit of the object pass (`RLE_Blit` and `Bit_Blit` into the map view) as a `GpuBlit`: colour indices, depth values, translator, lighting remap and blend kind. |
| GPU renderer | `code/gpumap.cpp` | Draws a recorded pass with the map shaders into an offscreen target, reads it back and compares it with the software picture. |
| Shaders | `code/shaders/mapdraw/` | `vs_mapdraw.sc` and `fs_mapdraw.sc`, compiled into `vs_mapdraw.bin.h` and `fs_mapdraw.bin.h`. |

The two autotest steps that drive it are documented in [AUTOTEST.md](AUTOTEST.md): `gpureplay`
replays the record in software, and `gpucheck` draws it on the GPU and logs `GPU_DIFF` lines.

## Limits

- The shaders exist for Direct3D 11 only. On other renderers `gpucheck` logs one
  `GPU_DIFF unavailable` line and draws nothing.
- Copies through a translator match the software picture exactly. Darken and the 25%, 50% and 75%
  translucent kinds finish in the hardware blend in 8-bit colour. The software blends floor each
  halved or quartered term; the shader floors the source term and offsets the target term by its
  average flooring loss. In a 1,500-frame large battle at 1920x1080 this left about 180 pixels per
  pass differing from the software replay, almost all by one step in a 565 channel; the largest
  difference was 4 steps.
- Particle pixels and spotlights are drawn without blits, so the record does not hold them.

## Rebuilding the shaders

The `.bin.h` files are checked in, so a normal build does not need `shaderc`. After editing a `.sc`
file, rebuild both headers from the repository root with a `shaderc` built from
`thirdparty/bgfx.cmake`:

```bash
shaderc -f code/shaders/mapdraw/vs_mapdraw.sc -o vs_mapdraw.bin --type vertex --platform windows -p s_5_0 -O 3 --varyingdef code/shaders/mapdraw/varying.def.sc -i thirdparty/bgfx.cmake/bgfx/src
```

```bash
shaderc -f code/shaders/mapdraw/fs_mapdraw.sc -o fs_mapdraw.bin --type fragment --platform windows -p s_5_0 -O 3 --varyingdef code/shaders/mapdraw/varying.def.sc -i thirdparty/bgfx.cmake/bgfx/src
```

Then write each `.bin` into its header as a `static const uint8_t vs_mapdraw_dxbc[]` or
`fs_mapdraw_dxbc[]` array, keeping the comment line at the top. `shaderc` rejects the `vs_5_0` and
`ps_5_0` profile names; use `s_5_0` for both.
