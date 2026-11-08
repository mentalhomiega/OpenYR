---
title: Show a shut-down vehicle with sparks and a dimmer draw
category: fix
release: 0.2.0
targets:
- type: key
  id: PoweredUnit
  effect: changed
credit:
- MentalHomiega
---

A unit shut down by [`PoweredUnit=yes`](/keys/poweredunit/) now throws sparks near itself about every 30 game frames, and a voxel unit is drawn at half brightness while it is shut down. Before, a shut-down Allied Robot Tank (`ROBO`) showed neither. Shape units are not dimmed.
