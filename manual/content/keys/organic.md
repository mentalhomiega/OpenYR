---
key: Organic
summary: Makes a unit die under the Iron Curtain and the chronosphere instead of being protected or moved.
see_also: [IronCurtainDuration, Teleporter, "system:superweapons"]
when_omitted:
  kind: context-dependent
  note: An InfantryType starts at yes. Every other type starts at no.
---

An `Organic=yes` vehicle or aircraft covered by a [`Type=IronCurtain`](/keys/type/#scope-superweapontype) superweapon takes damage equal to its full strength instead of being protected. Its armor can reduce that damage, so it may survive. Infantry always die under the Iron Curtain, whatever this key says, and a structure is protected even with it set.

An `Organic=yes` unit the [chronosphere](/systems/superweapons/#chronosphere) picks up is destroyed instead of moved, unless its type is also [`Teleporter=yes`](/keys/teleporter/). An `Organic=no` infantryman is moved like a vehicle.

```ini title="rulesmd.ini"
[MYBEAST] ; example VehicleType
Organic=yes
```
