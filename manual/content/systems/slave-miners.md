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

The slaves are checked every 10 frames. Each slave inside a deployed miner comes out at the free cell nearest the middle of the structure's right edge and looks for ore within [`SlaveMinerSlaveScan`](/keys/slaveminerslavescan/#scope-global-rules) cells. It walks to the ore it finds and shovels for [`HarvestRate`](/keys/harvestrate/#scope-infantrytype) frames per bail. When its cell runs out, it looks for more.

A slave goes back once it carries its `Storage`, or when it finds no ore in range. Within one cell of the middle of the right edge it goes inside and its ore is paid to the owner as a harvester's would be, including the [`PurifierBonus`](/keys/purifierbonus/) bonus. It rests inside for [`SlaveReloadRate`](/keys/slavereloadrate/#scope-aircrafttype) frames and comes out again with full strength.

While the miner is a vehicle, its slaves walk back to it, unload their ore and stay inside.

## Losing and freeing slaves

A slave that is killed is replaced after [`SlaveRegenRate`](/keys/slaveregenrate/#scope-aircrafttype) frames. A [`Slaved=yes`](/keys/slaved/#scope-infantrytype) slave takes no movement orders from the player while it works for a miner.

When the miner is destroyed, its slaves inside are lost with it. Slaves in the field drop their ore, stop working, and join the house of the object that destroyed the miner. Without a known destroyer they join the `Neutral` house, or stay with their owner when the scenario has no `Neutral` house. Either way they become ordinary infantry that take orders. [`SlavesFreeSound`](/keys/slavesfreesound/#scope-global-rules) plays at the miner when at least one slave is freed. A miner removed from the game without being destroyed frees its slaves as if it had no destroyer.
