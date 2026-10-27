---
title: Reveal cloaked objects within a sensor array's SensorsSight
category: fix
release: 0.2.0
targets:
- type: key
  id: SensorArray
  effect: changed
- type: key
  id: SensorsSight
  effect: changed
- type: system
  id: cloaking
  effect: changed
credit: [MentalHomiega]
---

A sensor array, such as the Yuri Psychic Sensor, now senses the cells within its `SensorsSight` of it, as in Yuri's Revenge. Before, it sensed out to `CloakRadiusInCells`, which defaults to 20, so a submerged submarine 16 to 20 cells from a Yuri Psychic Sensor was shown to its owner. Sensing that reaches beyond `SensorsSight` no longer shows cloaked objects.
