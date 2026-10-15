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

- `HDRHighlights=Enhanced` and HDR output is active. `Apply_HDR_Highlights()`
  runs again once the renderer has started, so the masks follow the renderer.
- `EmissiveDebugView=yes`, which is a developer key. It also shows the masks in
  place of the pictures.

`Apply_HDR_Highlights()` in `code/options.cpp` makes this decision from
`Video_HDR_Active()`, which reports whether the renderer started with an HDR10
back buffer.

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

## How the HDR branch uses the mask

The presenter draws the mask into a target the size of the drawable, once per
present, and the composite samples that target at the colour's UV. The draws
follow the colour exactly, so the mask and the picture agree:

1. Clear the mask target to zero at the start of each HDR frame.
2. Under a layer, draw the layer's mask over the layer's rectangle. Where the
   layer has no mask, zero that rectangle instead.
3. Draw the frame's mask over the frame's rectangle. Under a layer, the frame
   uses the keyed mask: its red channel is the frame mask, and its alpha is
   zero where the frame has the layer key. The blend then keeps the layer's
   mask at keyed pixels.
4. The composite binds the mask target as `s_mask`, point sampled, only while
   the frame drew a mask. Otherwise it reads no mask.

The masks are point sampled throughout. The mask target is BGRA8, with the mask
in the red channel. SDR output clears the target and never reads it.

## Limits

- The mask follows draws in order, so a later draw clears the mask under the
  pixels it covers, whether or not that draw is emissive.
- When the surface has a depth buffer, the mask applies the depth test to every
  RLE draw, including blitters that do not test depth. A pixel the colour pass
  drew over a building can therefore have no mask, or a mask where the colour
  pass drew nothing.
- Lasers and electric bolts are stamped along their centre line, one pixel
  wide. Their glow is not stamped.
- Voxels write the mask beside each palette index, with `Emissive_Voxel_Highlight()`
  on the shade row. The table does not separate specular rows from diffuse rows
  above 16, so both feed the mask.
- Light posts have a name default (`NAPOST_A`, `NAPOST_AD`, `NAPOST_B`). The
  Chronosphere and Iron Curtain effects have no separate rule. They are emissive
  through their flags, such as `Translucency=50`, or through an `Emissive=` key.
- Overlays drawn after the frame, such as menus, do not clear the mask under
  them. A light source can glow through an overlay's pixels.

## Where the code is

| Piece | File |
| --- | --- |
| Plane storage, defaults table, blend maths | `code/emissive.h`, `code/emissive.cpp` |
| Blit and surface hooks | `code/emissive_surface.cpp`, `code/blit.cpp`, `code/dsurface.cpp`, `code/gscreen.cpp` |
| Anim, laser and bolt draws | `code/anim.cpp`, `code/laser.cpp`, `code/ebolt.cpp`, `code/bullet.cpp` |
| Presenting the masks | `code/video.cpp`, `code/bgfxbackend.cpp`, `code/shaders/composite.sc` |
| Voxel highlights | `code/voxdrsys.cpp`, `code/voxlib.cpp` |
| Settings | `code/options.cpp`, `code/options.h` |
| Unit test | `tests/emissive/emissivetest.cpp` |
