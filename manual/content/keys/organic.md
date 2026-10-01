---
key: Organic
summary: Makes a vehicle or aircraft die under the Iron Curtain instead of being protected.
see_also: [IronCurtainDuration, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

An `Organic=yes` vehicle or aircraft covered by a [`Type=IronCurtain`](/keys/type/#scope-superweapontype) superweapon takes damage equal to its full strength instead of being protected. Its armor can reduce that damage, so it may survive.

```ini title="rulesmd.ini"
[MYBEAST] ; example VehicleType
Organic=yes
```

Infantry always die under the Iron Curtain, whatever this key says, and a structure is protected even with it set.
