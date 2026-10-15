# Emissive mask

The emissive mask tells the HDR composite which pixels are light sources, so
they can go brighter than paper white. This note owns the mask's format, its
settings, and the formula the composite uses. The HDR output work owns the
swap chain and the paper white and peak values.

## Format

- One byte per pixel, the same width and height as the picture it follows.
  Zero is an ordinary pixel. 255 is the strongest boost.
- The frame mask follows the frame, which is the presented picture. The layer
  mask follows the map layer that a zoomed map is drawn from. A layer mask
  exists only while the layer is drawn.
- The mask is uploaded as an `R8` texture with the picture, in
  `code/bgfxbackend.cpp` (`_FrameMask`, `_LayerMask`). `Backend_Present` takes
  the masks through `BackendEmissive`.
- Masks are kept only while `Emissive_Is_Enabled()` is true. Then the planes
  are allocated and written. While it is false, no plane exists and no pixel
  is written.

## When masks are on

Masks are on when either condition holds:

- `HDRHighlights=Enhanced` and HDR output is active. This build has no HDR
  output, so this condition is always false here.
- `EmissiveDebugView=yes`, which is a developer key. It also shows the masks in
  place of the pictures.

`Apply_HDR_Highlights()` in `code/options.cpp` makes this decision. The HDR
output branch replaces its stand-in `HDR_Output_Active()` with the monitor
state.

## Composite formula

    out_nits = min(peak, paper_white * linear_rgb * (1 + mask/255 * (peak/paper_white - 1)))

- `linear_rgb` is the decoded linear value of the colour channel, from 0 to 1.
- `paper_white` and `peak` are in nits. `peak` is the display's peak
  brightness, clamped between paper white and about 10000 nits.
- A mask of 0 gives `paper_white * linear_rgb`, the ordinary picture.
- A mask of 255 gives `peak` for a full-brightness pixel. Lower values scale
  linearly between the two.
- Classic highlights ignore the mask. The composite then uses the ordinary
  formula, which is the same as a mask of 0.

`Emissive_Gain()` and `Emissive_Output_Nits()` in `code/emissive.cpp` compute
the same formula on the CPU. The unit test `tests/emissive/emissivetest.cpp`
checks them.

## Sampling

Sample the mask at the same texel as the colour. Use point sampling, so the
edge of a light source stays on the pixel it came from. If the colour is
filtered, the mask edge can differ by a fraction of a pixel. That is accepted.

## What the HDR branch must add

The HDR composite pass samples the colour frame and the frame mask at the same
UV. When the layer is drawn, it does the same with the layer mask.

1. Add a second sampler for the mask. Bind `_FrameMask.Handle` only while
   `_FrameMask.Uploaded` is true. Bind `_LayerMask.Handle` the same way for the
   layer. When a mask is not uploaded, the composite must read a mask of 0.
2. In the composite fragment shader, read `mask = texture(maskSampler, uv).r`
   and apply the formula above with the paper white and peak uniforms.
3. Pass Classic as a uniform that forces the mask to 0.
4. The composite must keep the layer key. Where the frame is the key colour,
   the frame mask is 0, because the map area is filled with the key before the
   layer is drawn.

The current `Submit_Quad` binds one texture. The composite pass needs a second
texture binding on the same quad.

## Limits

- The mask follows draws in order, so a later draw clears the mask under the
  pixels it covers, whether or not that draw is emissive.
- When the surface has a depth buffer, the mask applies the depth test to every
  RLE draw, including blitters that do not test depth. A pixel the colour pass
  drew over a building can therefore have no mask, or a mask where the colour
  pass drew nothing.
- Lasers and electric bolts are stamped along their centre line, one pixel
  wide. Their glow is not stamped.
- Voxel highlights are not wired. `Emissive_Voxel_Highlight()` maps a shade row
  to a strength, but the voxel drawer does not write the mask yet.
- Light posts and the Chronosphere and Iron Curtain effects have no separate
  rule. They are emissive only when their anim types match the defaults in
  `EmissiveDefaults`, or when an `Emissive=` key marks them.

## Where the code is

| Piece | File |
| --- | --- |
| Plane storage, defaults table, blend maths | `code/emissive.h`, `code/emissive.cpp` |
| Blit and surface hooks | `code/emissive_surface.cpp`, `code/blit.cpp`, `code/dsurface.cpp`, `code/gscreen.cpp` |
| Anim, laser and bolt draws | `code/anim.cpp`, `code/laser.cpp`, `code/ebolt.cpp`, `code/bullet.cpp` |
| Presenting the masks | `code/video.cpp`, `code/bgfxbackend.cpp` |
| Settings | `code/options.cpp`, `code/options.h` |
| Unit test | `tests/emissive/emissivetest.cpp` |
