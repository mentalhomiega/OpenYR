---
key: ThirdBaseDefenses
scope: global-rules
label: Third side base defenses
summary: The structures a computer house of the third or any later side in `[Sides]` may plan as base defenses.
see_also: [AlliedBaseDefenses, SovietBaseDefenses, AntiAirValue, AntiArmorValue, AntiInfantryValue, "system:ai-base-building"]
when_omitted:
  kind: value
  value: ""
---

When a computer house [fills a defense node](/systems/ai-base-building/#base-defenses) in its base plan, it chooses only from the list for the side of its own country. This list serves the third or any later side listed in `[Sides]`. An entry still needs a value above `0` in the category being filled, set by [`AntiAirValue`](/keys/antiairvalue/), `AntiArmorValue` or `AntiInfantryValue`, and must pass the other candidate tests. With the list empty, the side's computer houses delete their defense nodes and build no defenses.

```ini title="rulesmd.ini"
[AI]
ThirdBaseDefenses=YAGGUN,NATBNK
```
