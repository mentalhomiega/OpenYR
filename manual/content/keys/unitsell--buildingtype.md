---
key: UnitSell
scope: buildingtype
summary: "Whether a vehicle or aircraft at this structure can be sold there."
see_also: [UnitRepair, Unsellable]
when_omitted:
  kind: inherited
  note: The value of UnitRepair.
---

With `UnitSell=yes`, the sell tool sells a vehicle or aircraft that is on the structure's pad in the structure's radio contact, as it sells one on a service depot. With `no`, the unit cannot be sold there, even on a structure with `UnitRepair=yes`.

```ini title="rulesmd.ini"
[GAAIRC] ; example BuildingType
UnitSell=no
```
