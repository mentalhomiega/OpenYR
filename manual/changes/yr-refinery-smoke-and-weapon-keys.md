---
title: Port the refinery smoke, eruption, sabotage cursor and projectile keys
category: feature
release: 0.2.0
targets:
- type: key
  id: RefinerySmokeParticleSystem
  effect: changed
- type: key
  id: RefinerySmokeFrames
  effect: added
- type: key
  id: RefinerySmokeOffsetOne
  effect: added
- type: key
  id: RefinerySmokeOffsetTwo
  effect: added
- type: key
  id: RefinerySmokeOffsetThree
  effect: added
- type: key
  id: RefinerySmokeOffsetFour
  effect: added
- type: key
  id: SabotageCursor
  effect: added
- type: key
  id: IsRadEruption
  effect: added
- type: key
  id: Scalable
  effect: added
- type: key
  id: FirersPalette
  effect: added
- type: key
  id: SpawnDelay
  effect: added
  scope: bullettype
- type: enum
  id: ActionType
  effect: changed
- type: system
  id: projectile-flight
  effect: changed
credit:
- MentalHomiega
---

A structure that sets `RefinerySmokeParticleSystem=` now starts that particle system at each set `RefinerySmokeOffsetOne=` to `RefinerySmokeOffsetFour=` position on every harvester payout, for `RefinerySmokeFrames=` frames. Before, the system was read and never started.

`SabotageCursor=yes` on a weapon shows the demolition cursor over what an infantryman would attack with it. `IsRadEruption=yes` throws eight arching radiation beams around the firer. `FirersPalette=yes` draws a shape projectile in its firer's color scheme. `Scalable=yes` lengthens a projectile type's trail delay while many are in flight. A projectile's art section can set `SpawnDelay=`; the trail was always every third frame before.
