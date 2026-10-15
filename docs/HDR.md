# HDR output

We send the picture to an HDR display as HDR10 when Windows has HDR on for the game's display. The
game still draws its art in SDR. The picture is the same SDR picture, converted to HDR10 at the end,
so colours match SDR and nothing is brightened beyond SDR white.

## Settings

All three keys are in `[Video]`. Each has a manual page.

| Key | Default | Applies |
| --- | --- | --- |
| [`HDROutput`](/keys/hdroutput/) | `Automatic` | At the next start |
| [`HDRPaperWhite`](/keys/hdrpaperwhite/) | `Auto` (Windows SDR content brightness, or 200 nits) | At once |
| [`HDRPeakBrightness`](/keys/hdrpeakbrightness/) | `Auto` (the display's reported peak, or 1000 nits) | At once |

The Display page of the settings shows the three as the HDR output list and the Paper white and
Peak brightness sliders. The sliders show the effective value when the key is `Auto`.

## Detection

`HDR_Query_Window_Display` (`code/hdrdisplay.cpp`) finds the DXGI output on the window's monitor and
reads `IDXGIOutput6::GetDesc1`. Windows HDR is on when the output's colour space is
`DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020`. The same call gives the peak and full-frame
luminance. The SDR content brightness comes from `DisplayConfigGetDeviceInfo` with
`DISPLAYCONFIG_DEVICE_INFO_GET_SDR_WHITE_LEVEL`, which counts in thousandths of 80 nits. Each start
writes one `HDR:` line to the debug log.

## Back buffer

With HDR output on, `Backend_Init` asks bgfx for `RGB10A2` with `BGFX_RESET_HDR10`. bgfx sets the
swap chain's colour space to `DXGI_COLOR_SPACE_RGB_FULL_G2084_NONE_P2020` for that format
(`thirdparty/bgfx.cmake/bgfx/src/dxgi.cpp`, `Dxgi::updateHdr10`). If bgfx refuses the format, or
reports no HDR10 support, the renderer starts again with the SDR back buffer and the log says why.

## Pipeline

The game renders its frame in software as before. With HDR output on, or with a colour adjustment
that is not the identity, the frame and the interface are drawn into a BGRA8 scene target instead
of the window. The composite pass then draws that target to the window with
`code/shaders/composite.sc`. The steps run in this order:

1. Decode the sRGB value to linear light.
2. Apply the 3x3 colour matrix, row-major, to the linear RGB.
3. Multiply by the brightness.
4. Raise to the gamma exponent.
5. HDR only: multiply by the paper white, convert Rec.709 primaries to Rec.2020 (BT.2087), and limit
   each channel to the peak.
6. Encode: ST.2084 (PQ) for HDR10, or the sRGB curve for SDR.

Steps 2 to 4 run only when the colour adjustment is not the identity. The identity is the matrix
`1 0 0 0 1 0 0 0 1`, brightness `1` and gamma `1`. With the identity and SDR output, the frame goes
straight to the window as before, so SDR output does not change. Backend_Set_Colour_Adjust sets
the adjustment; Video_Set_Colour_Adjust sets it and presents again.

The CPU reference for steps 1, 5 and 6 is `code/hdrcolour.cpp`, and its test checks known values.
The colour adjustment in `HDR_Apply_Colour_Adjust` follows the same order as steps 2 to 4.

## Screenshots

Screenshots are SDR images in every mode. With HDR output and the identity adjustment, the capture
copies the scene target into a readable texture with a blit and reads it back, then writes the TGA.
With an adjustment, the composite is drawn once more in SDR for the capture. bgfx's own screenshot
path cannot read an offscreen target on Direct3D 11, so the HDR path does not use it.

## Shader

`code/shaders/fs_composite.bin.h` holds the Direct3D 11 (`s_5_0`) bytecode of `composite.sc`. It
is generated, and we commit the result. To change the shader:

1. Build shaderc from `thirdparty/bgfx.cmake` with `BGFX_BUILD_TOOLS=ON`, and copy
   `d3dcompiler_47.dll` from `thirdparty/bgfx.cmake/bgfx/tools/bin/windows` beside it.
2. From the repository root, run
   `shaderc -f code/shaders/composite.sc -o fs_composite.bin --type fragment --platform windows -p s_5_0 -O 3 --varyingdef code/shaders/varying.def.sc -i thirdparty/bgfx.cmake/bgfx/src`.
3. Write the bytes of `fs_composite.bin` as `static const uint8_t fs_composite_dxbc[N]` in
   `fs_composite.bin.h`, with the comment line at the top of the file.

The shader ships for Direct3D 11 only. Direct3D 12, Vulkan and OpenGL get no composite program, so
HDR output stays off there.

## Limits

- Only Direct3D 11 (renderer `Auto` or `1`) gets HDR output.
- The peak is not written to the swap chain's HDR10 metadata. bgfx writes the metadata from the
  display's own description, and it exposes no call for another value.
- The game draws nothing above SDR white, so the peak limit changes no pixel yet.
- The HDR output option is read at start. A change to it needs a restart.
- Auto HDR, which Windows applies to games that use an 8-bit swap chain, is a Windows setting. We
  have not checked how it interacts with this path on Windows 11.

## Testing

The unit test `hdrcolour` checks the sRGB decode, the PQ encode at 0, 1, 80, 100, 200, 1000 and
10000 nits, the primaries, the paper white and peak limits, the adjustment order and the setting
resolution. It runs without a GPU or game data.

An autotest run with `HDROutput=On` takes a `windowshot`. Its TGA matches the SDR TGA of the same
frame byte for byte, which shows that the scene target and the readback carry the SDR picture
unchanged. A probe with a temporary readback of the PQ output matched a Python reference to within
one code value on every channel, for the plain frame and for an adjusted frame.

Testing on a display with HDR on needs a person at the display. Check that the picture has the same
brightness as SDR at the default paper white, and that the colours match SDR.
