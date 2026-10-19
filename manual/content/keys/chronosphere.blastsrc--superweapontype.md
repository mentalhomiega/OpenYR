---
key: Chronosphere.BlastSrc
scope: superweapontype
summary: "Names the animation that plays over the picked cell when units are chronoshifted."
see_also: ["system:superweapons"]
when_omitted:
  kind: computed
  note: "[General] ChronoBlast"
---

When the warp fires, this animation plays over the cell the chronosphere picked, in place of `ChronoBlast`. The key is read from the `Type=ChronoSphere` weapon that picked the units, not from the `Type=ChronoWarp` weapon.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Chronosphere.BlastSrc=CHRONOFD
```

See [Chronosphere](/systems/superweapons/#chronosphere).
