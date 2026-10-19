---
key: IvanBomb.CanDetonateTimeBomb
summary: "Whether the player can set off a bomb this weapon plants by hand."
see_also: [IvanBomb, "system:ivan-bombs", CanDetonateTimeBomb]
when_omitted:
  kind: context-dependent
  note: "`CanDetonateTimeBomb` from `[CombatDamage]`."
---

The player sets off the bomb by selecting only the vehicle or soldier carrying it and clicking it, as [`CanDetonateTimeBomb`](/keys/candetonatetimebomb/) describes. A weapon that does not write the key plants a bomb that follows `CanDetonateTimeBomb`.

```ini title="rulesmd.ini"
[MyBombWeapon] ; example WeaponType
Warhead=MyIvanBombWH
IvanBomb.CanDetonateTimeBomb=yes
```
