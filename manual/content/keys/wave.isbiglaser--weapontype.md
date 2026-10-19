---
key: Wave.IsBigLaser
scope: weapontype
summary: "The same key as IsBigLaser under its Ares spelling."
see_also: [IsBigLaser, "Wave.Color"]
when_omitted:
  kind: inherited
  note: The value of IsBigLaser.
---

`Wave.IsBigLaser=yes` makes the wave of an `IsLaser=yes` weapon the wide one, as [`IsBigLaser=yes`](/keys/isbiglaser/) does. The key written last wins.

```ini title="rulesmd.ini"
[MyLaser] ; example WeaponType
Wave.IsBigLaser=yes
```
