---
key: HDRPaperWhite
scope: client-settings
label: Paper white
see_also: [HDROutput, HDRPeakBrightness]
when_omitted:
  kind: value
  value: Auto
  note: Auto follows the SDR content brightness Windows reports for the display, or 200 nits when it reports none.
---

`HDRPaperWhite=` sets the brightness, in nits, at which SDR white is drawn when HDR output is on. It is written in `[Video]`. The value is `Auto` or a whole number. A number is limited to 80 to 480 nits.

At `Auto`, the game uses the SDR content brightness that Windows is set to for the display. That is the brightness Windows gives ordinary content in HDR mode. When Windows reports no value, the game uses 200 nits.

The setting has no effect while HDR output is off. The Paper white slider on the Display page of the settings changes it at once, while the game runs.
