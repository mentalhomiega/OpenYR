---
key: Convert.Script
summary: The vehicle type this vehicle becomes when its team runs a Convert type script line.
see_also: ["Convert.Land", "Convert.Water", "Convert.Deploy", Locomotor]
when_omitted:
  kind: value
  value: "none, so the vehicle keeps its type on the script line"
---

`Convert.Script` is an Ares key. When a team runs the [Convert type](/mapping/missions/tmission-convert-type/) script line, each vehicle in it becomes the named vehicle type where it stands. The conversion keeps the owner, rank and place and the same share of health, as [`Convert.Deploy`](/keys/convert.deploy/) does, and a type with another [`Locomotor`](/keys/locomotor/) is refused. A type that names itself does nothing.

The key applies to vehicles. A team's infantry and aircraft are not converted.

```ini title="rulesmd.ini"
[ROBO] ; example VehicleType
Convert.Script=ROBOW
```
