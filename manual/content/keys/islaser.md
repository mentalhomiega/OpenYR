---
key: IsLaser
summary: Draws a beam from the mounting to the target as the weapon fires.
see_also: ["LaserInnerColor", "LaserOuterColor", "LaserOuterSpread", "LaserDuration", "IsBigLaser", "Charges"]
when_omitted:
  kind: value
  value: "no"
---

`IsLaser=yes` draws a beam from the mounting to the target each time the weapon fires. The beam is only a visual effect. The weapon still fires its projectile, and the projectile deals all the damage when it detonates.

```ini title="rules.ini"
[MyObeliskRay] ; example WeaponType
IsLaser=yes
LaserInnerColor=255,0,0
LaserOuterColor=128,0,0
LaserOuterSpread=20,40,40
LaserDuration=15
Projectile=LLine ; a BulletType, registered by a weapon naming it as its Projectile
```

## What is drawn

A laser shot draws two effects along the line from the mounting to the target's aim point:

- The beam: a thin core line in [`LaserInnerColor`](/keys/laserinnercolor/), with two glow lines beside it in [`LaserOuterColor`](/keys/laseroutercolor/). [`LaserOuterSpread`](/keys/laserouterspread/) varies the glow color each frame, and the beam lasts [`LaserDuration`](/keys/laserduration/) frames. While the frame rate is below [`DetailMinFrameRateNormal`](/keys/detailminframeratenormal/#scope-global-rules), the beam is drawn as plain shaded lines, as at the lowest detail level.
- A screen glow along the same line, drawn only at the high [detail level](/keys/detaillevel/#scope-client-settings). It brightens only the red of whatever lies beneath it, whatever colors the beam uses, and fades out over about 22 frames. [`IsBigLaser=yes`](/keys/isbiglaser/) widens it.

:::caution[The beam's settings come from the first weapon slot]
Whichever slot fired, the beam's colors, spread, duration and glow width come from the weapon in the object's first slot. A laser weapon in the second slot therefore draws its beam in the first weapon's colors. If the first-slot weapon is not a laser, the beam uses that weapon's color and duration settings, which are usually the defaults.
:::

## Effects on the shot

A laser weapon's projectile starts at the mounting and takes its first flight turn later in the frame it is fired, as any other projectile does. [The shot, step by step](/systems/firing-geometry/#the-shot-step-by-step) lists the firing order.

On a structure, each laser shot resets the turret's charge animation to its first frame. The structure also loses its turret charge, unless it has more than one round of [`Ammo`](/keys/ammo/) left. [`Charges`](/keys/charges/) covers what that means for a charging weapon.
