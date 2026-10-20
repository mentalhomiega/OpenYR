---
key: Convert.Water
scope: unittype
summary: "The vehicle type this vehicle becomes on a beach or water cell."
see_also: ["Convert.Land", "Convert.Deploy", Locomotor]
when_omitted:
  kind: value
  value: "none, so the vehicle keeps its type on water"
---

Each time a vehicle of this type arrives at the center of a cell that is beach or water, it becomes the named vehicle type where it stands. The conversion keeps the owner, rank and place and the same share of health, as [`Convert.Deploy`](/keys/convert.deploy/) does, and a type with another [`Locomotor`](/keys/locomotor/) is refused. A type that names itself does nothing.

The type for water usually names the type for land in [`Convert.Land`](/keys/convert.land/), which makes a pair that changes with the ground under it.

```ini title="rulesmd.ini"
[ROBO] ; example VehicleType, the land form
Convert.Water=ROBOW

[ROBOW] ; the water form
Convert.Land=ROBO
```
