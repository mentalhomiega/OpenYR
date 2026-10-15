---
key: HDROutput
scope: client-settings
label: HDR output
see_also: [HDRPaperWhite, HDRPeakBrightness, Renderer]
when_omitted:
  kind: value
  value: Automatic
  note: Automatic sends HDR10 only while Windows has HDR on for the display the game is on.
---

`HDROutput=` chooses whether the game sends its picture to the display as HDR10. It is written in `[Video]` and takes one of three values:

| Value | Effect |
| --- | --- |
| `Automatic` | HDR10 while Windows has HDR on for the game's display. Otherwise the SDR picture. |
| `Off` | The SDR picture, whatever Windows reports. |
| `On` | HDR10 even when Windows reports HDR off. On a display that is not in HDR mode the picture looks washed out, so use this value only to test. |

HDR10 output needs the Direct3D 11 interface, which is [`Renderer`](/keys/renderer/) `0` (chosen automatically) or `1`. With another interface, or when the renderer cannot start with an HDR10 back buffer, the game draws the SDR picture and logs the reason.

The colours match the SDR picture. SDR white is drawn at the brightness set by [`HDRPaperWhite`](/keys/hdrpaperwhite/), and the picture is not stretched beyond it. The screenshot command saves SDR images in every mode.

The setting is read when the game starts. The HDR output list on the Display page of the settings stores a new value for the next start.
