---
title: Add the Ring1Glow key
category: feature
release: 0.2.0
targets:
- type: key
  id: Ring1Glow
  effect: added
credit:
- MentalHomiega
---

`Ring1Glow=` in `[AudioVisual]` sets whether an animation of type `RING1` is drawn as an expanding glow on the ground instead of its shape. The default, `no`, keeps the shape, which is what the shipped Yuri's Revenge draws. The glow follows the drawing code that `gamemd.exe` holds for it and never enables.
