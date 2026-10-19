---
key: IvanBomb.Detachable
summary: "Whether a bomb this weapon plants can be removed by a bomb disarming weapon."
see_also: [IvanBomb, "system:ivan-bombs", BombDisarm]
when_omitted:
  kind: value
  value: "yes"
---

A weapon with a [`BombDisarm=yes`](/keys/bombdisarm/) warhead removes the bomb from an object only when the weapon that planted the bomb has `IvanBomb.Detachable=yes` or does not write the key. Otherwise the disarming shot is spent and the bomb stays on the object.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.Detachable=no
```
