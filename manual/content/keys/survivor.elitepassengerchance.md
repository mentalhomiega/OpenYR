---
key: Survivor.ElitePassengerChance
summary: The odds in percent that each passenger of this elite transport survives its destruction.
see_also: ["Survivor.RookiePassengerChance", "Survivor.VeteranPassengerChance", Crashable, Passengers, "system:transports"]
when_omitted:
  kind: value
  value: "-1"
  note: The stock rule, in which a passenger that can enter the vehicle's cell gets out.
---

`Survivor.ElitePassengerChance` is a whole number of percent from `0` to `100`, and a trailing `%` is accepted. When a elite vehicle of this type is destroyed, each passenger rolls this chance once. A passenger that fails is killed, credited to whatever destroyed the vehicle, and one that succeeds gets out onto the vehicle's cell, or onto the nearest free cell when that is taken. A passenger that finds no free cell is killed.

A negative or absent value keeps the stock rule, in which every passenger that can enter the vehicle's cell gets out. The stock restrictions still apply with this key set. Forced damage, a vehicle bomb's death, a height above 208 leptons and [`Crashable=yes`](/keys/crashable/) kill every passenger whatever the chance is.

[Losing the transport](/systems/transports/#losing-the-transport) describes what a passenger that gets out does.

```ini title="rulesmd.ini"
[APC] ; example VehicleType
Survivor.RookiePassengerChance=50% ; half of a rookie one's passengers live
Survivor.ElitePassengerChance=100%
```
