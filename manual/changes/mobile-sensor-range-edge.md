---
title: Leave the edge of SensorsSight out of mobile sensor coverage
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

A mobile sensor no longer senses the cells exactly `SensorsSight` away. It senses only the cells whose distance is less than `SensorsSight`, as in Yuri's Revenge, so `SensorsSight=8` no longer reaches the cell eight cells straight out along a row or column.
