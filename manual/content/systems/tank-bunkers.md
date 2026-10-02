---
title: Tank bunkers
summary: "How a vehicle enters a bunker, fights from it with better damage, rate of fire and range, and leaves it."
category: combat-targeting
keys:
  - Bunker
  - Bunkerable
  - BunkerDamageMultiplier
  - BunkerROFMultiplier
  - BunkerWeaponRangeBonus
  - BunkerWallsUpSound
  - BunkerWallsDownSound
  - OccupyHeight
---

A [`Bunker=yes`](/keys/bunker/#scope-buildingtype) structure holds one vehicle. The Tank Bunker works this way: a tank parked inside fires harder, faster and farther than it does in the open.

## Entering

The player can send a vehicle into a bunker when all of these hold:

- The bunker and the vehicle have the same owner, and the bunker is finished, not being sold, and empty.
- The vehicle is [`Bunkerable=yes`](/keys/bunkerable/#scope-aircrafttype), has a primary weapon, is on the ground and carries no passengers.

The cursor shows the enter action over such a bunker and the no-entry action over one that cannot take the vehicle. The vehicle drives onto the bunker and stops at its middle. The bunker's `SpecialAnim` and `SpecialAnimTwo` art animations play as its walls rise, in their damaged forms when the bunker is at yellow health or lower, and [`BunkerWallsUpSound`](/keys/bunkerwallsupsound/#scope-global-rules) plays. Computer players do not use bunkers.

## Fighting from a bunker

While it is inside, the vehicle stays selectable and on the map. It takes no damage except from warheads with [`PenetratesBunker=yes`](/keys/penetratesbunker/#scope-warheadtype), and those warheads leave the bunker unharmed; any other attack has to destroy the bunker first. It does not drive out toward a target beyond its reach; it forgets the target instead, unless a human player has it guard an area. Its shots gain three bonuses:

| Key | Effect |
| --- | --- |
| [`BunkerDamageMultiplier`](/keys/bunkerdamagemultiplier/#scope-global-rules) | Multiplies the damage of each shot. |
| [`BunkerROFMultiplier`](/keys/bunkerrofmultiplier/#scope-global-rules) | Divides the delay after each burst. |
| [`BunkerWeaponRangeBonus`](/keys/bunkerweaponrangebonus/#scope-global-rules) | Adds whole cells to the reach of weapons without arcing projectiles. |

[`OccupyHeight`](/keys/occupyheight/#scope-buildingtype) in the bunker's art section lets the vehicle show over the bunker's floor.

## Leaving

Any order that moves the vehicle off the bunker's cells takes it out. The walls go down and [`BunkerWallsDownSound`](/keys/bunkerwallsdownsound/#scope-global-rules) plays. A vehicle destroyed inside leaves the bunker empty with its walls down. A bunker that is destroyed or sold lets its vehicle go where it stands.
