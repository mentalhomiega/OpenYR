---
title: Give light posts and voxel highlights emissive defaults
category: feature
release: 0.2.0
targets:
- type: key
  id: Emissive
  scope: animtype
  effect: changed
credit: [MentalHomiega]
---

[`Emissive`](/keys/emissive/#scope-animtype) has a default for the light post lamp animations, and it scales the picture's strength by its translucency. Voxel shade rows above neutral now feed the emissive mask, so bright parts of voxel units can rise above paper white on HDR output.
