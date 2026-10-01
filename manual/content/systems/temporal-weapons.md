---
title: Temporal weapons
summary: "How a temporal weapon freezes its target and erases it, and what makes it let go."
category: combat-targeting
keys:
  - Temporal
  - Warpable
  - WarpAway
  - ChronoSparkle1
  - OpenToppedWarpDistance
  - IsRadBeam
  - ChronoBeamColor
related:
  - type: system
    id: mind-control
  - type: system
    id: transports
---

An object whose primary weapon has a [`Temporal=yes`](/keys/temporal/) warhead erases what it hits instead of damaging it. The object gets this ability when it is placed on the map, from the primary weapon it has at that moment.

```ini title="rulesmd.ini"
[MYLEGIONNAIRE] ; example InfantryType
Primary=MyNeutronRifle

[MyNeutronRifle]
Damage=8            ; warp taken off the target each frame
Warhead=MyChronoBeam
IsRadBeam=yes       ; draws the beam in ChronoBeamColor

[MyChronoBeam]
Temporal=yes
```

## Freezing the target

When the shot hits, the target lets go of every object it holds by [mind control](/systems/mind-control/#letting-go), and is then frozen for as long as the warp lasts. A frozen object cannot move, fire or carry out orders, drops its target and destination, and lets go of any target it was itself warping. It is drawn half see-through, with [`ChronoSparkle1`](/keys/chronosparkle1/) playing beside it every 24 frames. Weapons other than temporal ones cannot fire at it.

A shot freezes nothing of a [`Warpable=no`](/keys/warpable/) type, under the Iron Curtain or a force shield, or a vehicle still standing in the weapons factory it is in contact with, though it still frees what that object holds by mind control. A firer that is itself being warped cannot start a warp.

## Erasing it

The warp starts at ten times the target type's [`Strength`](/keys/strength/#scope-aircrafttype). Each frame it drops by the firer's weapon `Damage`, plus the weapon `Damage` of every other temporal firer on the same target, so several firers erase a target faster. When the warp runs out, [`WarpAway`](/keys/warpaway/) plays where the target stood and the target is removed, counting as a kill for the firer holding the warp at that moment. A structure's garrison is removed with it.

While a firer holds a warp, it does not fire at the same target again.

## Letting go

A firer lets go of its target when it moves into another cell, when another of its shots hits, or when it is destroyed or leaves the map, for example by boarding a transport. A firer riding an [open-topped transport](/systems/transports/#firing-from-an-open-topped-transport) also lets go once the target is more than [`OpenToppedWarpDistance`](/keys/opentoppedwarpdistance/) cells away.

If other firers are still warping the target when the firer holding the warp lets go, the one that joined most recently takes over the warp left. Otherwise the target is freed unharmed.

Yuri's Revenge also warns the player when their harvester is being warped and shuts a warped structure down. Neither is done yet.
