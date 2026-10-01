---
key: OccupyROFMultiplier
summary: The factor a garrisoned structure divides the delay between its shots by.
see_also: [OccupyDamageMultiplier, OccupyWeaponRange, CanOccupyFire, "system:garrisons"]
when_omitted:
  kind: value
  value: "1.0"
---

A [`CanOccupyFire=yes`](/keys/canoccupyfire/) structure takes the delay of the weapon it fires, divides it by the number of soldiers inside, and then divides the result by this value. A value above `1.0` makes every garrison fire faster.

```ini title="rulesmd.ini"
[CombatDamage]
OccupyROFMultiplier=2.0
```

Both divisions drop any fraction. A value of `0` or below skips the second division, so the delay depends only on the number of occupants.
