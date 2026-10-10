---
key: BuildTimeAircraftMult
summary: The factor on how long a house of this country takes to build aircraft.
see_also: [BuildTimeInfantryMult, BuildTimeUnitsMult, BuildTimeBuildingsMult, BuildTimeMultiplier, BuildSpeed, "system:production"]
when_omitted:
  kind: value
  value: "1.0"
---

The build time of an aircraft for a house of this country is multiplied by this value, so a value above 1 builds more slowly and one below 1 builds faster. The value applies in every game type and the time is rounded down to a whole number after the multiply. [How long it takes](/systems/production/#how-long-it-takes) places it among the other build-time factors. The price does not change.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
BuildTimeAircraftMult=0.75 ; example: this country builds these objects in 25% less time
```
