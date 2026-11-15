---
key: Chronosphere.BlastDest
scope: superweapontype
summary: "Names the animation that plays over the target cell when units are chronoshifted."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] ChronoBlastDest"
---

When the warp fires, this animation plays over the target cell, in place of `ChronoBlastDest`. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.BlastDest=CHRONOTG
```

See [Chronosphere](/systems/superweapons/#chronosphere).
