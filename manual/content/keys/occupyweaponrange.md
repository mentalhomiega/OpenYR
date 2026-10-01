---
key: OccupyWeaponRange
summary: How far a garrisoned structure searches for targets, in cells.
see_also: [OccupyDamageMultiplier, OccupyROFMultiplier, CanOccupyFire, Range, "system:garrisons"]
when_omitted:
  kind: value
  value: "5"
---

A [`CanOccupyFire=yes`](/keys/canoccupyfire/) structure with occupants searches the cells around it for targets. The search reaches this many cells, plus one, plus half the shorter side of its foundation rounded down. It takes the place of the search distance the weapon's range would give.

```ini title="rulesmd.ini"
[CombatDamage]
OccupyWeaponRange=6
```

The search only finds targets. A target must still be within the occupant weapon's [`Range`](/keys/range/#scope-weapontype) to be fired on, so a value larger than the weapons' reach gains nothing. A smaller value keeps the structure from noticing targets its weapons could reach.
