---
title: Disguises
summary: "How a spy takes on another soldier's look, and who sees through it."
category: combat-targeting
keys:
  - MakesDisguise
  - CanDisguise
  - Disguised
  - FireOnce
  - PermaDisguise
  - AlliedDisguise
  - SovietDisguise
  - ThirdDisguise
  - DisguiseWhenStill
  - DefaultMirageDisguises
  - DisguiseFakeBlinkTime
  - DisabledDisguiseDetectionPercent
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

The disguise shows only to players whose house is not an ally of the disguised object's owner; its owner and allies see it as it is. It lasts until the object copies another soldier, until a hit hurts it, or until the house it imitates leaves the game. We stop a unit that a spy hits from firing back while the spy looks like a soldier to that unit's house. The spy looks like a soldier to a house that is not an ally of the spy's house, when no detector covers the spy's cell and the house the spy copied is that house or an ally of the spy's house. The warhead does no damage, whether or not it disguises anything.

A `CanDisguise=yes` type with [`PermaDisguise=yes`](/keys/permadisguise/) is disguised from the start and never loses its disguise to damage. Whenever it has no other disguise, it looks like its owner's side's default soldier: [`AlliedDisguise`](/keys/allieddisguise/), [`SovietDisguise`](/keys/sovietdisguise/) or [`ThirdDisguise`](/keys/thirddisguise/), in its owner's colors.

An infantry type with [`Disguised=yes`](/keys/disguised/) is a separate, older disguise: it always looks like the rules' `Disguise` type to other houses.

A disguised soldier looks like a friend to a house when it wears that house's soldier, or an ally's soldier, and no [`DetectDisguise`](/keys/detectdisguise/) structure of that house covers its cell. The house's units then pass it over when they scan for targets, and do not fire back at it. A [`DetectDisguise=yes`](/keys/detectdisguise/) scanner sees through the disguise, and so does a computer house when [`AIDetectDisguise`](/keys/aidetectdisguise/) is set. A `PermaDisguise` soldier wears its owner's soldier, so the houses that are not its owner's allies attack it.

## Vehicles that hide as terrain

A [`DisguiseWhenStill=yes`](/keys/disguisewhenstill/) vehicle looks like a tree while it stands still: it picks one of the [`DefaultMirageDisguises`](/keys/defaultmiragedisguises/) terrain types at random and is drawn as that terrain to players whose house is not an ally of its owner. It drops the disguise as soon as it moves. On seven frames in eight, a soldier of a house that is not an ally standing in a neighboring cell also drops the disguise, and the vehicle cannot take a new one for [`InfantryBlinkDisguiseTime`](/keys/infantryblinkdisguisetime/) frames. The vehicle's shadow is still drawn while it is disguised. A unit that the vehicle hits does not fire back while the vehicle looks like terrain to that unit's house. A computer house's units pick the vehicle as an automatic target only while it blinks after a shot, and then only on a roll: [`DisguiseFakeBlinkTime`](/keys/disguisefakeblinktime/) frames follow each shot, and the roll is set by [`DisabledDisguiseDetectionPercent`](/keys/disableddisguisedetectionpercent/) for the side. A human house's units never pick it automatically, and units of a type with [`DetectDisguise=yes`](/keys/detectdisguise/) see through the disguise.

## Structures that see through disguises

A [`DetectDisguise=yes`](/keys/detectdisguise/) structure shows its owner the objects within [`DetectDisguiseRange`](/keys/detectdisguiserange/) cells as they are: an object that copied a soldier keeps its own look and name, and a still `DisguiseWhenStill` vehicle is drawn as a vehicle. The structure does this only if it is [operational](/systems/power/#defenses) when it is placed on the map; losing power later does not stop it, and gaining power later does not start it. It stops when it leaves the map, and a working one that is captured works for its new owner instead.

```ini title="rulesmd.ini"
[MYSENSOR] ; example BuildingType
DetectDisguise=yes
DetectDisguiseRange=8
```

A soldier in a cell that such a structure covers does not look like a friend to its owner, so the owner's units can pick it as a target. The same cells show the soldier as it is on screen. [`DetectDisguise`](/keys/detectdisguise/) on any object also lets it pick a disguised soldier as a target, as its key page describes.
