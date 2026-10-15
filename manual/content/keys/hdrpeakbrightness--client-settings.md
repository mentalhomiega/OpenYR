---
key: HDRPeakBrightness
scope: client-settings
label: Peak brightness
see_also: [HDROutput, HDRPaperWhite]
when_omitted:
  kind: value
  value: Auto
  note: Auto follows the peak luminance the display reports, or 1000 nits when it reports none.
---

`HDRPeakBrightness=` sets the highest brightness, in nits, that HDR output draws. It is written in `[Video]`. The value is `Auto` or a whole number. A number is limited to the paper white at the bottom and 10000 nits at the top, so a value below the [`HDRPaperWhite`](/keys/hdrpaperwhite/) is raised to it.

The game does not yet draw anything brighter than SDR white, so this limit leaves the picture unchanged. The game also does not write the peak into the HDR10 metadata of its back buffer. Windows and the display use the values they read from the monitor.

The Peak brightness slider on the Display page of the settings changes it at once, while the game runs.
