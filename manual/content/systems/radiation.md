---
title: Radiation
summary: "How a weapon with RadLevel leaves radiation on the ground, how it fades, and whom it hurts."
category: combat-targeting
keys:
  - RadLevel
  - RadDurationMultiple
  - RadLevelDelay
  - RadApplicationDelay
  - RadLevelMax
  - RadLevelFactor
  - RadSiteWarhead
  - Radiation
  - ImmuneToRadiation
  - RadLightDelay
  - RadLightFactor
  - RadTintFactor
  - RadColor
related:
  - type: system
    id: temporal-weapons
---

A weapon with a [`RadLevel`](/keys/radlevel/) above `0` leaves a patch of radiation where its projectile goes off. Vehicles, infantry and aircraft on the ground take damage from the radiation of the cell they stand in until it fades.

```ini title="rulesmd.ini"
[MyEruption] ; example Weapon
RadLevel=500
Warhead=MyEruptionWarhead

[MyEruptionWarhead]
CellSpread=3          ; the radiation reaches three cells out

[Radiation]
RadDurationMultiple=1 ; a level-500 patch lasts 500 frames
RadLevelDelay=90
RadApplicationDelay=16
RadLevelMax=500
RadLevelFactor=0.2    ; a cell at level 500 does 100 damage every 16 frames
RadSiteWarhead=MyRadSite
```

## Where the radiation goes

The patch is centered on the cell where the projectile went off and reaches the warhead's [`CellSpread`](/keys/cellspread/) in whole cells, plus half a cell. A cell gets the full `RadLevel` at the center, falling in a straight line to none at the edge of the reach, measured between cell centers including their height.

A shot that goes off in the center cell of an existing patch adds to that patch instead: the patch's level becomes what it has left plus the new `RadLevel`, and its fading starts over.

## Fading

A patch lasts [`RadDurationMultiple`](/keys/raddurationmultiple/) frames for each point of its level. Every [`RadLevelDelay`](/keys/radleveldelay/) frames, each of its cells loses an equal step of what the patch gave it, so the radiation runs out as the patch ends. Radiation from several patches on one cell adds up.

## Damage

Every [`RadApplicationDelay`](/keys/radapplicationdelay/) frames, each vehicle, soldier and landed aircraft takes damage from the radiation of its cell, counted up to [`RadLevelMax`](/keys/radlevelmax/) and multiplied by [`RadLevelFactor`](/keys/radlevelfactor/), rounded down. The damage goes through [`RadSiteWarhead`](/keys/radsitewarhead/) and has no attacker. Structures, objects in the air and objects inside transports take none, and neither does an [`ImmuneToRadiation=yes`](/keys/immunetoradiation/) type.

An `ImmuneToRadiation=yes` type also takes no damage from any warhead with [`Radiation=yes`](/keys/radiation/), whatever fired it.

## Glow

A patch lights the ground as far as its radiation reaches, brightest at its center. The light's brightness is the patch's level times [`RadLightFactor`](/keys/radlightfactor/), and its tint is [`RadColor`](/keys/radcolor/) scaled so `255` means full strength, times [`RadTintFactor`](/keys/radtintfactor/); each is capped at `2000`. Every [`RadLightDelay`](/keys/radlightdelay/) frames the brightness drops by an equal step and the tint shrinks with the time the patch has left, so the glow fades out with the patch.
