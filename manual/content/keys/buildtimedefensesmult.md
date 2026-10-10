---
key: BuildTimeDefensesMult
summary: The factor on how long a house of this country takes to build defenses.
see_also: [BuildTimeInfantryMult, BuildTimeUnitsMult, BuildTimeAircraftMult, BuildTimeMultiplier, BuildSpeed, "system:production"]
when_omitted:
  kind: value
  value: "1.0"
---

The build time of a defense, a structure whose [`BuildCat`](/keys/buildcat/) is `Combat` for a house of this country is multiplied by this value, so a value above 1 builds more slowly and one below 1 builds faster. The value applies in every game type and the time is rounded down to a whole number after the multiply. [How long it takes](/systems/production/#how-long-it-takes) places it among the other build-time factors. The price does not change.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
BuildTimeDefensesMult=0.75 ; example: this country builds these objects in 25% less time
```
