---
key: Convert.Land
scope: unittype
summary: "The vehicle type this vehicle becomes on a cell that is not beach or water."
see_also: ["Convert.Water", "Convert.Deploy", Locomotor]
when_omitted:
  kind: value
  value: "none, so the vehicle keeps its type on land"
---

Each time a vehicle of this type arrives at the center of a cell that is neither beach nor water, it becomes the named vehicle type where it stands. [`Convert.Water`](/keys/convert.water/) describes what the conversion keeps and when it is refused.

```ini title="rulesmd.ini"
[ROBOW] ; example VehicleType, the water form
Convert.Land=ROBO
```
