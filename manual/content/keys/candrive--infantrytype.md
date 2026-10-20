---
key: CanDrive
scope: infantrytype
summary: "Lets a soldier take over a vehicle that belongs to a house that can be driven from."
see_also: [CanBeDriven, "VehicleThief.EnterSound", VehicleThief, KillDriver]
when_omitted:
  kind: value
  value: "no"
---

A `CanDrive=yes` soldier can take over a vehicle when all of these hold:

- The vehicle belongs to another house whose [country](/keys/canbedriven/#scope-housetype) can be driven from. By default that means a house with `MultiplayPassive=yes`, such as the neutral and special houses that a [`KillDriver=yes`](/keys/killdriver/) warhead hands a vehicle to.
- The vehicle's type has [`CanBeDriven=yes`](/keys/canbedriven/#scope-aircrafttype).
- The vehicle's type is not an [`IsTrain=yes`](/keys/istrain/) type.

A player-controlled soldier then gets the capture cursor over the vehicle. When the soldier reaches the vehicle's cell on a capture order it enters, the vehicle passes to the soldier's house and the soldier is used up. The soldier does not become a driver the vehicle remembers, and `Operator` is not read. The soldier plays its [`VehicleThief.EnterSound`](/keys/vehiclethief.entersound/) where it enters.

The key does not make computer-controlled soldiers look for such vehicles. A soldier with [`VehicleThief=yes`](/keys/vehiclethief/) already takes vehicles from any other house, so the key adds nothing for it.

```ini title="rulesmd.ini"
[ENGINEER] ; example InfantryType
CanDrive=yes
```
