---
key: Survivor.RookiePilotChance
summary: The odds in percent that each pilot of a destroyed rookie vehicle of this type escapes.
see_also: ["Survivor.Pilots", CrewEscape, "Survivor.VeteranPilotChance", "Survivor.ElitePilotChance"]
when_omitted:
  kind: inherited
  note: "[General] CrewEscape, as a fraction."
---

`Survivor.RookiePilotChance` is a whole number of percent from `0` to `100`, and a trailing `%` is accepted. Each of the vehicle's [`Survivor.Pilots`](/keys/survivor.pilots/) rolls succeeds with this chance when the vehicle destroyed is a rookie one. A negative or absent value uses [`CrewEscape`](/keys/crewescape/) for that rank instead.

The rank is the one the vehicle holds when it is destroyed. [Veterancy and promotion](/systems/veterancy/) explains how objects gain rank.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
Crewed=yes
Survivor.RookiePilotChance=100% ; a rookie one always leaves a pilot
```
