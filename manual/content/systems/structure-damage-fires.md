---
title: Structure damage fires
summary: "Fires that burn at set points on a structure while its health is low, drawn in front of it."
category: buildings-economy
keys:
  - DamageFireTypes
  - ConditionYellow
  - ConditionRed
  - CanBeOccupied
related:
  - type: system
    id: building-animations
  - type: system
    id: destruction-and-debris
  - type: system
    id: garrisons
  - type: system
    id: network-synchronization
---

A structure burns while its health is low. Each fire is an animation from [`DamageFireTypes`](/keys/damagefiretypes/), pinned to a point that the structure's art sets. A type with no point set never burns.

## When a structure burns

A structure burns while its health, as a fraction of its maximum strength, is at or below [`ConditionYellow`](/keys/conditionyellow/). A structure with [`CanBeOccupied=yes`](/keys/canbeoccupied/) burns only at or below [`ConditionRed`](/keys/conditionred/).

The structure checks its health on every update. It lights its fires on the update that finds the health at or below the threshold, and puts them out on the update that finds it above. A structure that is already damaged when it appears burns from its first update.

The fires also go out when the structure is destroyed or leaves the map. A damaged structure that returns to the map burns again.

## Where the fires burn

The structure's art section sets up to eight points, `DamageFireOffset0` to `DamageFireOffset7`. Each is an `X,Y` pair of screen pixels measured from the structure's drawing point, a positive `X` to the right and a positive `Y` downward. A missing `Y` counts as 0.

```ini title="artmd.ini"
[MYBUILDING] ; the art section for an example BuildingType
DamageFireOffset0=0,-20
DamageFireOffset1=40,-10
```

The points are read in order and stop at the first one that is missing, so numbers after a gap are ignored. A type with no `DamageFireOffset0` has no points.

## What each fire is

The fires take their types from `DamageFireTypes` in turn. The first fire takes a type chosen at random, and each later fire takes the next entry in the list, wrapping from the last entry back to the first. Each fire then starts on a random frame of its animation. The random choices are the same for every player in a multiplayer game.

Each fire is drawn in front of the structure. Its depth adjustment replaces the animation type's own [`ZAdjust`](/keys/zadjust/) and is

```
min(0, (3 × (Y − 15 × (width + height)) ÷ 2) − 10)
```

where `Y` is the point's Y offset and `width` and `height` are the structure's footprint in cells, without bib cells. The product of three and the bracket is halved, rounding down. A negative value brings the animation toward the viewer, as [building animations](/systems/building-animations/#placement-and-draw-order) describes, so a fire is never drawn behind the structure.

## Where these keys are read

`DamageFireTypes` is read from `[General]` in the rules file. The `DamageFireOffset` points are read from the structure's art section, not from the rules file.
