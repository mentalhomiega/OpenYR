---
key: IsBigLaser
summary: Read for a laser weapon, but it changes nothing.
see_also: ["IsLaser", "LaserDuration"]
when_omitted:
  kind: value
  value: "no"
---

The game reads `IsBigLaser` and includes it in a weapon's checksum, but nothing draws or behaves differently when it is set. A laser beam has no separate glow for it to widen. [`IsLaser`](/keys/islaser/) describes what is drawn.

```ini title="rules.ini"
[MyObeliskRay] ; example WeaponType
IsLaser=yes
IsBigLaser=yes ; no effect
```
