---
key: DamSmkOffScrnRel
summary: Reads the damage smoke and spark offset as a screen offset, so X and Y follow the isometric view.
see_also: [DamageSmokeOffset, DamageParticleSystems]
when_omitted:
  kind: value
  value: "no"
---

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
DamageSmokeOffset=300,0,40 ; the offset as three numbers
DamSmkOffScrnRel=yes ; read the offset on the screen
```

With `DamSmkOffScrnRel=no`, the [`DamageSmokeOffset`](/keys/damagesmokeoffset/) is a world offset and is used as written. The smoke and the sparks start there.

With `yes`, X and Y are projected through the isometric view and cut to whole pixels. Each of them then loses ten times Z. The height is the negative of the truncated value of ten times the square root of two times Z when Z is zero or more, and the positive of that value when Z is below zero. The three results are read as leptons, so the pixel counts for X and Y are used as lepton distances.

The key applies to both the spark burst and the smoke that [`DamageParticleSystems`](/keys/damageparticlesystems/) start from the same offset.
