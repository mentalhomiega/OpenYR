---
key: Chronosphere.AffectsIronCurtain
scope: superweapontype
summary: "Chooses whether a chronoshift moves units under the Iron Curtain."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the chronosphere moves a unit that is under the Iron Curtain, where it otherwise stays. A unit already being chronoshifted still stays. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.AffectsIronCurtain=yes
```

See [Chronosphere](/systems/superweapons/#chronosphere).
