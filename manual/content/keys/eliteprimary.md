---
key: ElitePrimary
summary: The WeaponType an elite object fires in place of its primary.
see_also: [EliteSecondary, Primary, ElitePrimaryFireFLH, "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite primary weapon, an elite object keeps firing its primary weapon.
---

Once an object reaches elite rank, it uses this weapon in place of its primary for target selection, range checks, reload delay and firing. [`EliteSecondary`](/keys/elitesecondary/) does the same for the secondary weapon. [The elite weapons](/systems/veterancy/#the-elite-weapons) covers the rest of the substitution.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Primary=MyCannon     ; each names its own weapon section
ElitePrimary=MyEliteCannon
```

The elite primary fires from [`ElitePrimaryFireFLH`](/keys/eliteprimaryfireflh/), which defaults to the primary weapon's offset.

:::caution[An upgrade's weapon replaces this one]
A structure with an upgrade installed uses the upgrade's primary weapon when the upgrade supplies one. The structure's `ElitePrimary` weapon then never applies, whatever the structure's rank.
:::

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key and reads its weapons from a [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) instead.
