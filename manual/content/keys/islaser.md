---
key: IsLaser
summary: Draws a beam from the mounting to the target as the weapon fires.
see_also: ["LaserInnerColor", "LaserOuterColor", "LaserOuterSpread", "LaserDuration", "IsHouseColor", "IsBigLaser", "Charges"]
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

A laser shot draws one beam along the line from the mounting to the target's aim point. The beam is a thin core line in [`LaserInnerColor`](/keys/laserinnercolor/), with two glow lines one pixel to either side of it in [`LaserOuterColor`](/keys/laseroutercolor/). [`LaserOuterSpread`](/keys/laserouterspread/) varies the glow color each frame. The beam lasts [`LaserDuration`](/keys/laserduration/) frames, and its strength falls evenly from full to nothing over them, so it dims as it ages. The lines add their colors to the pixels beneath them in proportion to that strength. At the lowest [detail level](/keys/detaillevel/#scope-client-settings) they are plain solid lines that do not dim.

No other effect is drawn with the beam, and [`IsBigLaser`](/keys/isbiglaser/) changes nothing. A weapon with [`IsHouseColor=yes`](/keys/ishousecolor/) draws a thick beam in the house color instead of this one.

The beam takes its settings from the weapon that fired it.

## Effects on the shot

A laser weapon's projectile starts at the mounting and takes its first flight turn later in the frame it is fired, as any other projectile does. [The shot, step by step](/systems/firing-geometry/#the-shot-step-by-step) lists the firing order.

On a structure, each laser shot resets the turret's charge animation to its first frame. The structure also loses its turret charge, unless it has more than one round of [`Ammo`](/keys/ammo/) left. [`Charges`](/keys/charges/) covers what that means for a charging weapon.
