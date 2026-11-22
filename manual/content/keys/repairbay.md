---
key: RepairBay
summary: The BuildingTypes that a vehicle on the Repair mission, or a damaged computer aircraft, heads for to be repaired.
see_also: ["system:repair"]
when_omitted:
  kind: value
  value: "none"
---

```ini title="rules.ini"
[General]
RepairBay=GADEPT,NADEPT   ; GADEPT and NADEPT stand in for the BuildingTypes the mod uses

[GADEPT]
UnitRepair=yes      ; without this, the named type turns every arrival away
```

A comma-separated list. A name that is not a BuildingType is skipped. A vehicle on the [`Repair` mission](/reference/enums/mission/) heads for the nearest building of any listed type that meets all of these conditions:

- it belongs to the vehicle's house;
- it is not already serving another object;
- it lies in the same [movement zone](/glossary/#movement-zone) as the cell the vehicle stands in or is moving into;
- it accepts the vehicle.

If no such building exists but a busy one of a listed type does, the vehicle switches to its idle mission. With no listed building at all it keeps searching. A damaged computer aircraft looking for repair uses the same search, without the movement-zone test and with allied depots allowed. [Reaching the pad](/systems/repair/#reaching-the-pad) lists every route to a depot and its conditions, and the primary-factory exception to taking the nearest building.

Only the listed types are searched for. A player can still use a depot of another type by moving a vehicle onto it. A damaged computer vehicle also runs a separate search, which covers every [`UnitRepair=yes`](/keys/unitrepair/) building regardless of this setting.

Set [`UnitRepair=yes`](/keys/unitrepair/) on each type named here. Without it, the building refuses every vehicle.

This setting does not decide whether a selected player-controlled aircraft can enter a clicked building. That direct order accepts any idle, empty `UnitRepair=yes` building or helipad.
