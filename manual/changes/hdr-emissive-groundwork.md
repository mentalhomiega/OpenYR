---
title: Add emissive masks and the HDR highlights setting
category: feature
release: 0.2.0
targets:
- type: key
  id: HDRHighlights
  effect: added
- type: key
  id: EmissiveDebugView
  effect: added
- type: key
  id: Emissive
  scope: animtype
  effect: added
- type: key
  id: Emissive
  scope: weapontype
  effect: added
credit:
- MentalHomiega
---

`HDRHighlights` in `[Video]` of `RA2MD.INI` sets whether light sources can be drawn brighter than paper white on HDR output. This build has no HDR output, so `Enhanced` has no visible effect.

The emissive mask groundwork exists. `Emissive` in an animation type's section sets that animation's strength. In a weapon section of `rules.ini`, it sets the beam or bolt strength.

`EmissiveDebugView=yes` in `[Video]` is a developer key. It shows the masks in place of the art, and the red channel holds each mask value.

The voxel specular default, 128, is not applied yet. Light posts and the Chronosphere and Iron Curtain effects have no separate rule, so they are emissive only through `Emissive` or the defaults.
