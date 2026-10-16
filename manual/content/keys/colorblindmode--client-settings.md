---
key: ColorBlindMode
scope: client-settings
label: Colour-blind mode
see_also: [ColorBlindStrength, Brightness, Gamma, HDROutput]
when_omitted:
  kind: value
  value: "Off"
---

`ColorBlindMode=` chooses a colour-blind correction for the picture the game shows. It is written in `[Video]` and takes one of four values:

| Value | Correction for |
| --- | --- |
| `Off` | Nothing. The picture is unchanged. |
| `Protanopia` | Missing red cones. Red and green are hard to tell apart. |
| `Deuteranopia` | Missing green cones. Red and green are hard to tell apart. |
| `Tritanopia` | Missing blue cones. Blue and yellow are hard to tell apart. |

The value is read in any letter case. Any other text selects `Off`.

The correction changes the colours of the whole picture, including the interface, not only the units. It applies on SDR and HDR output. It needs the Direct3D 11 interface, which is [`Renderer`](/keys/renderer/) `0` (chosen automatically) or `1`; on another interface the picture is drawn without the correction.

[`ColorBlindStrength`](/keys/colorblindstrength/) sets how much of the correction applies. The Colour-blind mode list on the Display page of the settings writes this key, and the change takes effect when the page is accepted with OK.
