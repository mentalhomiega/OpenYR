---
key: UnitReload
summary: Makes the building rearm the object docked with it, one ammunition point at a time.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "no"
---

`UnitReload=yes` makes a building rearm the objects docked with it, every docked object in turn on a building with several [docks](/keys/numberofdocks--buildingtype/). Each gains one point of [`Ammo`](/keys/ammo/) every [`ReloadRate`](/keys/reloadrate/) interval until its ammunition is full. Rearming is free. An object with full ammunition that is damaged is then repaired a step at a time, at the same cost per step as at a repair depot; once it is full and undamaged, the building releases it.

A helipad rearms the aircraft that land on it only if it also sets this flag.

A building with both `UnitReload=yes` and [`UnitRepair=yes`](/keys/unitrepair/) repairs and never gives the one-point rearming, because the building runs only [the first service its flags match](/systems/repair/#unitreload-is-a-different-service). Such a depot still [refills a `ManualReload=yes` object for free](/systems/repair/#what-a-depot-does-for-free).

When another aircraft asks to dock, a parked aircraft with full ammunition moves to a nearby cell to make room for it. Otherwise the parked object keeps its place and the building refuses the newcomer.
