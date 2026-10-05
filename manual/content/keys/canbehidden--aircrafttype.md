---
key: CanBeHidden
scope: aircrafttype
label: 'Marked when hidden'
see_also: [CanHideThings, Behind]
when_omitted:
  kind: value
  value: "yes"
---

`CanBeHidden=no` stops an object of this type from being marked as hidden when it stands behind a [`CanHideThings=yes`](/keys/canhidethings/#scope-buildingtype) structure. With `yes`, the object gets the hidden marker while its cell is covered. Aircraft are never marked, whatever this key says.

The game reads this key from the art section named after the type's ID, not from the section its `Image=` points to.

```ini title="artmd.ini"
[MYTANK] ; example art section, named after the VehicleType's ID
CanBeHidden=no ; never marked, even behind a tall structure
```
