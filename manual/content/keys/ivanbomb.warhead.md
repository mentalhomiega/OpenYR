---
key: IvanBomb.Warhead
summary: "The warhead a bomb this weapon plants goes off with."
see_also: [IvanBomb, "system:ivan-bombs", IvanWarhead, IvanBomb.Damage]
when_omitted:
  kind: context-dependent
  note: "`IvanWarhead` from `[CombatDamage]`."
---

A bomb planted by a weapon that writes `IvanBomb.Warhead` explodes through that warhead, with that warhead's explosion animation. A weapon that does not write it plants a bomb that goes off through `IvanWarhead`. A bomb on an object that is off the map when its time comes, such as one inside a transport, still waits for the object to come back.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.Warhead=MyBombBlastWH
```
