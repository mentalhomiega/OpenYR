---
key: VehicleThief.Allowed
summary: Whether a vehicle thief can take vehicles of this type.
see_also: [VehicleThief, "system:capture"]
when_omitted:
  kind: value
  value: "yes"
---

The key is read on the victim. With `VehicleThief.Allowed=no`, soldiers with [`VehicleThief=yes`](/keys/vehiclethief/) cannot take a vehicle or landed aircraft of this type:

- A player-controlled thief gets the select cursor over it, as over an [`IsTrain=yes`](/keys/istrain/) type, and cannot be ordered to take it.
- A thief that already heads for it cannot step into its cell, so it does not take it.
- The thief does not keep it as its target while within 15 cells.

The key does not affect [`Thief=yes`](/keys/thief/) soldiers, which have their own [capture rules](/systems/capture/#stealing-a-vehicle). A vehicle that crushes a thief heading for it is also unaffected.

```ini title="rulesmd.ini"
[APOC] ; example VehicleType that thieves cannot take
VehicleThief.Allowed=no
```
