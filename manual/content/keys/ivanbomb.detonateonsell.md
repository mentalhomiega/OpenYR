---
key: IvanBomb.DetonateOnSell
summary: "Whether a bomb this weapon plants goes off when the structure carrying it is sold."
see_also: [IvanBomb, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "no, so selling the structure removes the bomb"
---

With `yes`, the bomb goes off as the sale of the structure starts, with its own damage and warhead, and the structure is then sold if it is still standing. With `no`, or when the weapon does not write the key, selling the structure removes the bomb without an explosion.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.DetonateOnSell=yes
```
