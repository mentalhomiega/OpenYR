---
title: Give mobile sensor coverage to any unit with SensorsSight
category: fix
release: 0.2.0
targets:
- type: key
  id: SensorsSight
  effect: changed
- type: system
  id: cloaking
  effect: changed
credit: [MentalHomiega]
---

A vehicle, infantryman or aircraft now marks cells as sensed for its owner from its `SensorsSight` alone. Before, it also needed `Sensors=yes`. The `Sensors` flag still ends cloaks by adjacency. A unit that leaves the map or changes owner lifts the cells it marked and no others.
