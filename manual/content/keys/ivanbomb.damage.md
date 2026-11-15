---
key: IvanBomb.Damage
summary: "The damage a bomb this weapon plants deals when it goes off."
see_also: [IvanBomb, "system:ivan-bombs", IvanDamage, IvanBomb.Warhead]
when_omitted:
  kind: context-dependent
  note: "`IvanDamage` from `[CombatDamage]`."
---

A bomb planted by a weapon that writes `IvanBomb.Damage` deals that damage through its warhead when it goes off. Any value written counts, including a negative one and `-1`. A weapon that does not write it plants a bomb that deals `IvanDamage`.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.Damage=420
```
