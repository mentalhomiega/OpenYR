---
key: BuildCat
scope: buildingtype
label: Building category
summary: The kind of structure this is, which decides the sidebar tab it is listed on.
see_also: ["system:sidebar"]
when_omitted:
  kind: value
  value: "DontCare"
---

`BuildCat` is one of `DontCare`, `Tech`, `Resource`, `Power`, `Infrastructure` or `Combat`, matched without regard to case. A `Combat` structure is listed on the sidebar's defenses tab; every other structure is listed on the buildings tab.

```ini title="rulesmd.ini"
[MyTower] ; example BuildingType
BuildCat=Combat
```

A value that is not one of the six names reads as `DontCare`, even when an earlier rules file set the key.
