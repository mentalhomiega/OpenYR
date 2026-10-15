---
key: Gamma
scope: client-settings
label: Gamma
see_also: [Brightness, ColorBlindMode]
when_omitted:
  kind: value
  value: "1.0"
---

`Gamma=` sets the exponent of the curve the picture's light follows, as a number. It is written in `[Video]`. `1.0` leaves the picture unchanged. Values above `1.0` darken the middle tones, and values below `1.0` lighten them. A number is limited to `0.5` to `2.0`. Text that is not a positive number selects `1.0`.

The gamma applies after the [`Brightness`](/keys/brightness/) scale and the colour-blind correction, to the light of each pixel. The setting changes the presented picture only.

The Gamma slider on the Display page of the settings writes this key, with two decimal places, and the change takes effect when the page is accepted with OK. On a renderer other than Direct3D 11 the picture is drawn without the change.
