---
title: Stop Chrono Miners crashing the game at a refinery dock
category: fix
release: 0.2.0
targets:
- type: key
  id: Teleporter
  effect: changed
credit: [Lucas]
---

A `Teleporter=yes` harvester sent onto its refinery's dock while driving no longer crashes the game. It switches back to its own locomotor once its current move step is over.
