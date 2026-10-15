---
title: Add a colour-blind correction for protanopia, deuteranopia and tritanopia
category: feature
release: 0.2.0
targets:
- type: key
  id: ColorBlindMode
  effect: added
- type: key
  id: ColorBlindStrength
  effect: added
credit: [MentalHomiega]
---

[`ColorBlindMode`](/keys/colorblindmode/) applies a correction to the whole picture for protanopia, deuteranopia or tritanopia. [`ColorBlindStrength`](/keys/colorblindstrength/) sets how much of it applies. The Display page of the settings has a Colour section with both controls. The correction works on SDR and HDR output, with the Direct3D 11 interface.
