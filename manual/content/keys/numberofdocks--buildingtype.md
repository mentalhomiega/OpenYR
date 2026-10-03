---
key: NumberOfDocks
scope: buildingtype
label: Number of docks
summary: How many objects a structure services at once, each on its own dock.
when_omitted:
  kind: value
  value: "1"
---

A structure keeps one radio contact per dock, so it can service that many objects at once. A value below `1` still gives the structure one contact. The stock Air Force Command sets `NumberOfDocks=4` and rearms four Harriers together.

On a [`Helipad=yes`](/keys/helipad/) or [`UnitRepair=yes`](/keys/unitrepair/) structure, each dock has its own landing spot: the structure's center plus that dock's `DockingOffsetN=` from the structure's art section, where `N` counts from `0`. An offset is three numbers in leptons (X, Y, height), and an offset the art does not set is `0,0,0`. With one dock, every visitor uses `DockingOffset0=`. With several, an object takes the first free dock when it makes contact and keeps it until contact ends; an object that holds no dock is sent to the center. With `NumberOfDocks=` of `0` or less, visitors use the center and no `DockingOffsetN=` is read.

An aircraft built at a pad with a free dock appears on that dock. When every dock is taken, a new aircraft arrives from the map edge instead. A [`UnitReload=yes`](/keys/unitreload/) pad rearms every docked aircraft in turn, not only the first.

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
