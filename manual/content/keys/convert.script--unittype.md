---
key: Convert.Script
scope: unittype
label: Vehicle conversion by script
see_also: ["Convert.Land", "Convert.Water", "Convert.Deploy", Locomotor]
when_omitted:
  kind: value
  value: "none, so the vehicle keeps its type on the script line"
---

`Convert.Script` is an Ares key. When a team runs the [Convert type](/mapping/missions/tmission-convert-type/) script line, each vehicle in it whose type sets the key becomes the named vehicle type where it stands. The conversion keeps the owner, rank, mission, team, selection and place and the same share of health, as [`Convert.Deploy`](/keys/convert.deploy/) does; ammo above the new type's maximum is cut. A type with another [`Locomotor`](/keys/locomotor/) is refused. A type that names itself does nothing.

```ini title="rulesmd.ini"
[ROBO] ; example VehicleType
Convert.Script=ROBOW
```
