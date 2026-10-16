---
key: Brightness
scope: client-settings
label: Brightness
see_also: [Gamma, ColorBlindMode, HDRPaperWhite]
when_omitted:
  kind: value
  value: "100"
---

`Brightness=` sets the brightness of the picture as a whole number of percent. It is written in `[Video]`. `100` leaves the picture unchanged. A number below `50` clamps to `50`, and a number above `200` clamps to `200`.

The brightness scales the light of the picture after the colour-blind correction and before the [`Gamma`](/keys/gamma/) curve. On HDR output it scales the light before it is set against [`HDRPaperWhite`](/keys/hdrpaperwhite/), so SDR white moves with it. The setting changes the presented picture only. The game's art, its rules and its saved games are not affected.

The Brightness slider on the Display page of the settings writes this key, and the change takes effect when the page is accepted with OK. On a renderer other than Direct3D 11 the picture is drawn without the change.
