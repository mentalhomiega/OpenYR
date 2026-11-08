---
title: Aim bolts and radiation beams at a structure's TargetCoordOffset
category: fix
release: 0.2.0
targets:
- type: key
  id: TargetCoordOffset
  scope: buildingtype
  effect: added
credit:
- MentalHomiega
---

A structure with `TargetCoordOffset=` in its rules section, such as the three stock naval yards, now takes electric bolts and radiation beams at that offset from its center, as in Yuri's Revenge. Before, the key was ignored and every bolt or beam ended on the center.
