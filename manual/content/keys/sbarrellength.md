---
key: SBarrelLength
summary: Parsed distance along the second weapon's barrel. No firing point or effect uses it.
no_effect: true
see_also: ["SecondaryFireFLH", "SBarrelThickness", "PBarrelLength"]
when_omitted:
  kind: value
  value: "0"
---

The value is read into the second weapon slot and stored. No firing point or effect reads it, so the projectile, the effects and the aim all stay at the mounting that [`SecondaryFireFLH`](/keys/secondaryfireflh/) and [`TurretOffset`](/keys/turretoffset/) set.

The elite secondary slot's [`EliteSBarrelLength`](/keys/elitesbarrellength/) defaults to this value. It has no effect either.
