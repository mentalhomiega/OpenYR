---
key: SovietBaseDefenses
scope: global-rules
label: Soviet base defenses
summary: The structures a computer house of the second side in `[Sides]` may plan as base defenses.
see_also: [AlliedBaseDefenses, ThirdBaseDefenses, AntiAirValue, AntiArmorValue, AntiInfantryValue, "system:ai-base-building"]
when_omitted:
  kind: value
  value: ""
---

When a computer house [fills a defense node](/systems/ai-base-building/#base-defenses) in its base plan, it chooses only from the list for the side of its own country. This list serves the second side listed in `[Sides]`. An entry still needs a value above `0` in the category being filled, set by [`AntiAirValue`](/keys/antiairvalue/), `AntiArmorValue` or `AntiInfantryValue`, and must pass the other candidate tests. With the list empty, the side's computer houses delete their defense nodes and build no defenses.

```ini title="rulesmd.ini"
[AI]
SovietBaseDefenses=NALASR,NAFLAK
```
