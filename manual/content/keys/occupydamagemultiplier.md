---
key: OccupyDamageMultiplier
summary: The factor applied to the damage of every shot a garrisoned structure fires.
see_also: [OccupyROFMultiplier, OccupyWeaponRange, CanOccupyFire, "system:garrisons"]
when_omitted:
  kind: value
  value: "1.0"
---

A [`CanOccupyFire=yes`](/keys/canoccupyfire/) structure multiplies the damage of each shot it fires by this value, after the owner's and the structure's firepower bonuses. The product is rounded down to a whole number.

```ini title="rulesmd.ini"
[CombatDamage]
OccupyDamageMultiplier=1.5
```

The multiplier applies to every occupant's weapon alike. It has no effect on a soldier firing outside a structure.
