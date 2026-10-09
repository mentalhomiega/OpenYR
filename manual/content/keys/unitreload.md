---
key: UnitReload
summary: Makes the building rearm the object docked with it, one ammunition point at a time.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "no"
---

`UnitReload=yes` makes a building rearm the objects docked with it, every docked object in turn on a building with several [docks](/keys/numberofdocks/). Each gains one point of [`Ammo`](/keys/ammo/) every [`ReloadRate`](/keys/reloadrate/) interval until its ammunition is full. Rearming is free. An object with full ammunition that is damaged is then repaired a step at a time, at the same cost per step as at a repair depot; once it is full and undamaged, the building releases it.

A helipad rearms the aircraft that land on it only if it also sets this flag.

A building with both `UnitReload=yes` and [`UnitRepair=yes`](/keys/unitrepair/) repairs and never gives the one-point rearming, because the building runs only [the first service its flags match](/systems/repair/#unitreload-is-a-different-service). Such a depot still [refills a `ManualReload=yes` object for free](/systems/repair/#what-a-depot-does-for-free).

An idle pad starts rearming when any docked object needs it, not only the object on the first dock.

When another aircraft asks to dock and no dock is free for it, the aircraft on the first dock moves to a nearby cell to make room if its ammunition is full. Otherwise that aircraft keeps its place and the building refuses the newcomer. An aircraft that already holds a dock, or finds one free, moves no one.
