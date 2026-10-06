---
key: NumberOfDocks
scope: buildingtype
label: Number of docks
summary: How many objects a structure services at once, each on its own dock.
when_omitted:
  kind: value
  value: "1"
---

A structure keeps one radio contact per dock, so that many objects can dock with it at once. A value below `1` still gives the structure one contact. A [`UnitReload=yes`](/keys/unitreload/) pad rearms and repairs every docked aircraft in turn. A [`UnitRepair=yes`](/keys/unitrepair/) structure repairs every docked object, each paying for its own steps. The stock Air Force Command sets `NumberOfDocks=4` and rearms four Harriers together.

On a [`Helipad=yes`](/keys/helipad/) or [`UnitRepair=yes`](/keys/unitrepair/) structure, each dock has its own landing spot: the structure's center plus that dock's `DockingOffsetN=` from the structure's art section, where `N` counts from `0`. An offset is three numbers in leptons (X, Y, height), and an offset the art does not set is `0,0,0`. With one dock, every visitor uses `DockingOffset0=`. With several, an object takes the first free dock when it makes contact and keeps it until contact ends; an object that holds no dock is sent to the center. With `NumberOfDocks=0`, visitors use the center.

When an aircraft asks to dock at a [`Helipad=yes`](/keys/helipad/) structure that also sets `UnitRepair=yes`, the structure orders it onto the pad if the aircraft stands more than half a cell from the structure's center. An aircraft that holds a dock is measured itself; one that holds none is measured by the object on the first dock.

When infantry asks to dock at a [`Hospital=yes`](/keys/hospital/) or [`Armory=yes`](/keys/armory/) structure, infantry that holds a dock is sent to the structure's own cell. Infantry that holds none disturbs the infantry on the first dock only when every dock is taken.

When a structure with several docks is [captured](/systems/capture/), every object on a dock is judged on its own: one standing within a quarter of a cell of its dock's spot changes owner with the structure, and any other is sent away.

A `UnitRepair=yes` structure starts repairing an object only when it stands within a quarter of a cell of the structure's center, or 150 leptons for a hovercraft. Keep every `DockingOffsetN=` within that distance on such a structure; otherwise the object on that dock is never repaired.

An aircraft built at a pad with a free dock appears on that dock. When every dock is taken, a new aircraft arrives from the map edge instead. During an [ion storm](/systems/ion-storms/), a new aircraft appears on a cell near the pad and does not dock, whether or not a dock is free.

```ini title="rulesmd.ini"
[GAAIRC] ; Air Force Command
NumberOfDocks=4
```

```ini title="artmd.ini"
[GAAIRC]
DockingOffset0=0,-128,0
DockingOffset1=0,128,0
DockingOffset2=256,-128,0
DockingOffset3=256,128,0
```
