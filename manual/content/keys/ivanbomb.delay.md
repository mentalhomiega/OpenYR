---
key: IvanBomb.Delay
summary: "The frames before a bomb this weapon plants goes off."
see_also: [IvanBomb, "system:ivan-bombs", IvanTimedDelay]
when_omitted:
  kind: context-dependent
  note: "`IvanTimedDelay` from `[CombatDamage]`."
---

A bomb planted by a weapon that writes `IvanBomb.Delay` goes off that many frames after it is planted, and its icon steps through its blocks over the same time. A weapon that does not write it plants a bomb with the `IvanTimedDelay` delay.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.Delay=300
```
