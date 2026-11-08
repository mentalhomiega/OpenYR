---
title: Naval yards refuse land vehicles and depots refuse ships
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitRepair
  effect: changed
- type: key
  id: BalloonHover
  effect: changed
credit:
- MentalHomiega
---

A `Naval=yes` structure refuses a docking request from an object that is not a ship, and any other structure refuses one from a ship, as in Yuri's Revenge. An object whose `MovementZone` is `Amphibious` is exempt. A player's land vehicle no longer gets the enter cursor over a naval yard. A `BalloonHover=yes` object is refused by every structure.
