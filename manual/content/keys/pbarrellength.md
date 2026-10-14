---
key: PBarrelLength
summary: Parsed distance along the first weapon's barrel. No firing point or effect uses it.
no_effect: true
see_also: ["PrimaryFireFLH", "PBarrelThickness", "ElitePBarrelLength", "SBarrelLength"]
when_omitted:
  kind: value
  value: "0"
---

The value is read into the first weapon slot and stored. No firing point or effect reads it, so the projectile, the effects and the aim all stay at the mounting that [`PrimaryFireFLH`](/keys/primaryfireflh/) and [`TurretOffset`](/keys/turretoffset/) set.

The elite primary slot's [`ElitePBarrelLength`](/keys/elitepbarrellength/) defaults to this value. It has no effect either.
