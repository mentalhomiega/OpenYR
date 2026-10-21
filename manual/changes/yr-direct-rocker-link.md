---
title: Link a direct rocker blast's firer and target
category: feature
release: 0.2.0
targets:
- type: key
  id: FallBackCoefficient
  scope: global-rules
  effect: added
- type: key
  id: Pushy
  scope: aircrafttype
  effect: added
credit: [MentalHomiega]
---

A direct rocker blast now links the firer to the vehicle it rocks. A linked infantry, vehicle or aircraft takes no click orders and does not move unless its type sets `Pushy=yes`, and a linked vehicle cannot fire. `FallBackCoefficient=` in `[AudioVisual]` sets how fast a linked vehicle settles back toward level.
