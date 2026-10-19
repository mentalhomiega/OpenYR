---
key: Survivor.Pilots
summary: How many pilots try to step out of this vehicle type when it is destroyed.
see_also: ["Survivor.RookiePilotChance", "Survivor.VeteranPilotChance", "Survivor.ElitePilotChance", CrewEscape, Crewed, "Survivor.Side0"]
when_omitted:
  kind: computed
  note: One for a Crewed=yes type that declares no Passengers capacity, otherwise none.
---

`Survivor.Pilots` is the number of rolls a destroyed vehicle of this type makes for a surviving crew member. Each roll uses the odds of the vehicle's rank, from [`Survivor.RookiePilotChance`](/keys/survivor.rookiepilotchance/), [`Survivor.VeteranPilotChance`](/keys/survivor.veteranpilotchance/) or [`Survivor.ElitePilotChance`](/keys/survivor.elitepilotchance/), or from [`CrewEscape`](/keys/crewescape/) when the rank's key is absent. Every roll that succeeds adds one soldier, whose type comes from [`Survivor.Side0`](/keys/survivor.side0/) and its siblings or from the owning side's crew.

The first soldier appears on the wreck's cell, and each later one on the nearest free spot. Each has between 5 and half its type's maximum strength.

The key applies only to a type with [`Crewed=yes`](/keys/crewed/). Setting it on a type that declares [`Passengers`](/keys/passengers/) lifts the rule that keeps a transport from leaving a crew. A value of `0` never produces one. A destroyed vehicle that [`CrewEscape`](/keys/crewescape/) lists as leaving no crew leaves none whatever this key says, and a vehicle a hijacker took returns the hijacker in place of the pilots.

```ini title="rulesmd.ini"
[APC] ; example VehicleType
Crewed=yes
Survivor.Pilots=2 ; two rolls
Survivor.RookiePilotChance=50 ; each succeeds half the time for a rookie vehicle
```
