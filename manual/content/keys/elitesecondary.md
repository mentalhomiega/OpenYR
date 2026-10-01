---
key: EliteSecondary
summary: The WeaponType an elite object fires in place of its secondary.
see_also: [ElitePrimary, Secondary, EliteSecondaryFireFLH, "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite secondary weapon, an elite object keeps firing its secondary weapon.
---

Once an object reaches elite rank, it uses this weapon in place of its secondary for target selection, range checks, reload delay and firing. [The elite weapons](/systems/veterancy/#the-elite-weapons) covers the rest of the substitution.

```ini title="rulesmd.ini"
[MYTROOPER] ; example InfantryType
Secondary=MyGrenade
EliteSecondary=MyEliteGrenade
```

The elite secondary fires from [`EliteSecondaryFireFLH`](/keys/elitesecondaryfireflh/), which defaults to the secondary weapon's offset.

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key and reads its weapons from a [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) instead.
