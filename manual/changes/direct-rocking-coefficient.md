---
title: Scale the rocking force of direct rocker blasts
category: feature
release: 0.2.0
targets:
- type: key
  id: DirectRockingCoefficient
  scope: global-rules
  effect: added
credit: [MentalHomiega]
---

`DirectRockingCoefficient=` in `[AudioVisual]` scales the force that a direct rocker blast gives a vehicle. The force is the blast's damage times this value, divided by 100, capped at 4, so a higher value rocks a vehicle harder. At `1`, the force matches a `Rocker` blast of the same damage, and `0` stops direct rocker blasts from rocking vehicles.
