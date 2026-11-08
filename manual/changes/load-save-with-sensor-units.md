---
title: Load saves that hold units with SensorsSight
category: fix
release: 0.2.0
targets:
- type: key
  id: SensorsSight
  effect: changed
credit: [MentalHomiega]
---

Loading a save made while a unit with `SensorsSight` was on the map, such as a destroyer or a dolphin, no longer crashes the game. The unit keeps the sensor coverage it had when the game was saved.
