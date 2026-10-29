---
title: Tip a sinking object slowly, by the way it faces
category: fix
release: 0.2.0
targets:
- type: key
  id: ShipSinkingWeight
  effect: changed
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A destroyed vehicle that sinks now tips by 0.01 radian a frame until it is 45 degrees over, as in Yuri's Revenge, so it takes about 80 frames to tip over fully. Before, it tipped three times as fast, and always the same way. It tips one way when it faces north, north-west or west and the other way when it faces any other direction. A crashing object of a `BalloonHover=yes` type is held to 45 degrees sideways either way.
