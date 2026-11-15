---
key: NoManualUnload
scope: aircrafttype
summary: "With yes, the player cannot unload this vehicle or aircraft by hand."
see_also: [NoManualEnter, Passengers]
when_omitted:
  kind: value
  value: "no"
---

A `NoManualUnload=yes` transport shows no deploy or no-deploy cursor over itself, so a click on it unloads nobody. Its passengers still leave when it is destroyed, within the limits of [`Survivor.PassengerChance`](/keys/survivor.rookiepassengerchance/), and an unload that a script or a computer player orders goes ahead. The key is meant for a vehicle that is not a real transport.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Passengers=1
NoManualUnload=yes
```
