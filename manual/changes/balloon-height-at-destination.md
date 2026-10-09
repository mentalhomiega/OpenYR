---
title: Keep a balloon's height above the terrain at its destination
category: fix
release: 0.2.0
targets:
- type: key
  id: BalloonHover
  effect: changed
credit: [MentalHomiega]
---

A BalloonHover unit that has reached its destination now keeps its height above the terrain and structures under it, as it does in flight. Over a building it hovers above the building. Before, it measured its height against the bare ground, so it hovered at the same height over a building as over open ground.
