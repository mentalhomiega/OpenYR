---
title: Do not paralyze a parasite that a dying victim releases
category: fix
release: 0.2.0
targets:
- type: system
  id: parasites
  effect: changed
credit: [MentalHomiega]
---

A Terror Drone that comes out of a victim which died or left the map can take a move order at once. Before, it was paralyzed for three times its weapon's `ROF`, so a move order given after the kill was ignored for that long. A drone driven out by a sonic hit or a teleport jump is still paralyzed.
