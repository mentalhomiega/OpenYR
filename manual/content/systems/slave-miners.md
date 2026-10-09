---
title: Slave miners
summary: "How a deployed slave miner's slaves gather ore on foot, carry it back for money and are replaced or freed."
category: buildings-economy
keys:
  - Enslaves
  - SlavesNumber
  - SlaveRegenRate
  - SlaveReloadRate
  - Slaved
  - HarvestRate
  - SlaveMinerSlaveScan
  - SlaveMinerShortScan
  - SlaveMinerLongScan
  - SlaveMinerScanCorrection
  - SlaveMinerKickFrameDelay
  - SlavesFreeSound
---

An object whose type names an InfantryType in [`Enslaves`](/keys/enslaves/#scope-aircrafttype) keeps [`SlavesNumber`](/keys/slavesnumber/#scope-aircrafttype) slaves of that type. They gather ore only while the object is a structure. Yuri's Slave Miner works this way: the vehicle deploys into its refinery near ore, and its slaves go out to mine.

```ini title="rulesmd.ini"
[MYMINER] ; example VehicleType
DeploysInto=MYMINERBASE
Enslaves=MYSLAVE
SlavesNumber=5

[MYMINERBASE] ; example BuildingType
UndeploysInto=MYMINER
Enslaves=MYSLAVE
SlavesNumber=5

[MYSLAVE] ; example InfantryType
Slaved=yes
HarvestRate=40
Storage=4
```

Deploying hands the vehicle's slaves to the structure, and packing up hands them back to the vehicle, so slaves keep their state through either change.

## Gathering

The slaves are checked every 10 frames. Each slave inside a deployed miner comes out at the free cell nearest the middle of the structure's right edge and looks for ore closer than [`SlaveMinerSlaveScan`](/keys/slaveminerslavescan/#scope-global-rules) cells. It walks to the ore it finds and shovels for [`HarvestRate`](/keys/harvestrate/#scope-infantrytype) frames per bail. When its cell runs out, it looks for more.

A slave goes back once it carries its `Storage`, or when it finds no ore in range. It walks to the free cell nearest that spot, and within one cell of it (or within two cells once it has stopped moving) it goes inside and its ore is paid to the owner as a harvester's would be, including the [`PurifierBonus`](/keys/purifierbonus/) bonus. It rests inside for [`SlaveReloadRate`](/keys/slavereloadrate/#scope-aircrafttype) frames and comes out again with full strength.

While the miner is a vehicle, its slaves walk back to it, unload their ore and stay inside.

## Moving the miner

A mobile miner on its guard or harvest mission sets out by itself after [`SlaveMinerKickFrameDelay`](/keys/slaveminerkickframedelay/#scope-global-rules) frames: a computer player's always, and a human player's when it stands on ore or has ore closer than [`SlaveMinerShortScan`](/keys/slaveminershortscan/#scope-global-rules) cells. It picks the richest ore in the nearest ring of cells that has any, closer than [`SlaveMinerLongScan`](/keys/slaveminerlongscan/#scope-global-rules) cells, and drives to the nearest place beside it that it can reach and where its structure fits on level ground; a footprint on a slope or a bridge never counts. When it stops, it deploys if the structure fits there. Otherwise it waits 30 frames, tries once more, and then chooses a new place. A computer player's miner also tells friendly units on the footprint to move off it each time the structure does not fit. The miner sets its own guard point on arrival, so it stays where it stopped until it deploys.

A deployed miner with no ore left closer than `SlaveMinerShortScan` cells of the middle of its right edge looks for ore the same way. If the new place is more than [`SlaveMinerScanCorrection`](/keys/slaveminerscancorrection/#scope-global-rules) cells away, it packs up, drives there and deploys again. Miners of human and computer players both do this.

## Losing and freeing slaves

A slave that is killed is replaced after [`SlaveRegenRate`](/keys/slaveregenrate/#scope-aircrafttype) frames. A [`Slaved=yes`](/keys/slaved/#scope-infantrytype) slave takes no movement orders from the player while it works for a miner.

When the miner is destroyed, its slaves inside are lost with it, and the object that destroyed the miner is credited with each one. Slaves in the field drop their ore, stop working, and join the house of the object that destroyed the miner. Without a known destroyer they join the house on the Civilian side, which is `Neutral` on the standard maps. Freed slaves become ordinary infantry that take orders. With no known destroyer and no house on the Civilian side, each slave in the field takes C4 damage instead. [`SlavesFreeSound`](/keys/slavesfreesound/#scope-global-rules) plays at the miner when at least one slave is freed. A miner removed from the game without being destroyed treats its slaves as if it had no destroyer.
