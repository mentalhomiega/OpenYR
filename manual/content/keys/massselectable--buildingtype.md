---
key: MassSelectable
scope: buildingtype
summary: "Whether a drag selection includes this structure."
see_also: [UndeploysInto]
when_omitted:
  kind: computed
  note: A structure that undeploys into a vehicle, or has SelectableCombatant=yes, is selected by a box; any other is not.
---

`MassSelectable=yes` puts the structure in a selection box that catches it, as it does a vehicle, and `no` leaves it out. Without the key, a structure that has `UndeploysInto=` and is not a construction yard is selected by a box, and so is one with `SelectableCombatant=yes`. A click on a structure selects it whatever this key says.

```ini title="rulesmd.ini"
[GAGAP] ; example BuildingType
MassSelectable=yes
```
