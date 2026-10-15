---
key: ColorBlindStrength
scope: client-settings
label: Colour-blind strength
see_also: [ColorBlindMode]
when_omitted:
  kind: value
  value: 100
---

`ColorBlindStrength=` sets how much of the [`ColorBlindMode`](/keys/colorblindmode/) correction applies, as a whole number of percent. It is written in `[Video]`. `0` applies no correction, `100` applies all of it, and a value in between blends the picture between the two. A number above `100` clamps to `100`, and a negative number clamps to `0`.

The Strength slider on the Display page of the settings writes this key. It has no effect while [`ColorBlindMode`](/keys/colorblindmode/) is `Off`.
