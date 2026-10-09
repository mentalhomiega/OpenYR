---
title: Wall the structures a computer house protects
category: feature
release: 0.2.0
targets:
- type: key
  id: ProtectWithWall
  effect: added
- type: key
  id: AIPickWallDefensePercent
  effect: added
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer house now walls a structure its rules flag with `ProtectWithWall`, such as a construction yard or a tech center. At each defense turn in its base plan, it draws a number and, when the number is below the slot's entry in `AIPickWallDefensePercent`, places a ring of walls one cell outside that structure instead of a defense. Before, a skirmish computer never placed these walls. The shipped rules set the percentages to 50, 25 and 10 for the hard, normal and easy slots. The save revision moves to 33, so saves from earlier builds are not compatible.
