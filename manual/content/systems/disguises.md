---
title: Disguises
summary: "How a spy takes on another soldier's look, and who sees through it."
category: combat-targeting
keys:
  - MakesDisguise
  - CanDisguise
  - Disguised
  - FireOnce
  - DisguiseWhenStill
  - DefaultMirageDisguises
  - InfantryBlinkDisguiseTime
  - DetectDisguise
  - DetectDisguiseRange
related:
  - type: system
    id: capture
---

An object of a [`CanDisguise=yes`](/keys/candisguise/) type that hits a soldier with a [`MakesDisguise=yes`](/keys/makesdisguise/) warhead takes on that soldier's look: its type's artwork and name, in its house's colors. A weapon with `Range=-2` reaches any target, so a spy can copy a soldier anywhere on the map.

```ini title="rulesmd.ini"
[MYSPY] ; example InfantryType
Primary=MyMakeupKit
CanDisguise=yes

[MyMakeupKit]
Range=-2       ; reaches any target
FireOnce=yes   ; one shot, then the spy drops the target
Warhead=MySnapshot

[MySnapshot]
MakesDisguise=yes
```

The disguise shows only to players whose house is not an ally of the disguised object's owner; its owner and allies see it as it is. It lasts until the object copies another soldier, or until the house it imitates leaves the game. The warhead does no damage, whether or not it disguises anything.

An infantry type with [`Disguised=yes`](/keys/disguised/) is a separate, older disguise: it always looks like the rules' `Disguise` type to other houses.

## Vehicles that hide as terrain

A [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle looks like a tree while it stands still: it picks one of the [`DefaultMirageDisguises`](/keys/defaultmiragedisguises/) terrain types at random and is drawn as that terrain to players whose house is not an ally of its owner. It drops the disguise as soon as it moves. On seven frames in eight, a soldier of a house that is not an ally standing in a neighboring cell also drops the disguise, and the vehicle cannot take a new one for [`InfantryBlinkDisguiseTime`](/keys/infantryblinkdisguisetime/) frames. The vehicle's shadow is still drawn while it is disguised.

## Structures that see through disguises

A [`DetectDisguise=yes`](/keys/detectdisguise/) structure shows its owner the objects within [`DetectDisguiseRange`](/keys/detectdisguiserange/) cells as they are: an object that copied a soldier keeps its own look and name, and a still `DisguiseWhenStill` vehicle is drawn as a vehicle. The structure does this only if it is [operational](/systems/power/#defenses) when it is placed on the map; losing power later does not stop it, and gaining power later does not start it. It stops when it leaves the map, and a working one that is captured works for its new owner instead.

```ini title="rulesmd.ini"
[MYSENSOR] ; example BuildingType
DetectDisguise=yes
DetectDisguiseRange=8
```

Detection affects only the look. [`DetectDisguise`](/keys/detectdisguise/) on any object also lets it pick a disguised soldier as a target, as its key page describes.
