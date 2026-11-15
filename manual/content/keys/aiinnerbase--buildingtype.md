---
key: AIInnerBase
scope: buildingtype
summary: "Makes the computer place the structure near the center of its base."
see_also: [CloakGenerator, AIBaseNormal]
when_omitted:
  kind: value
  value: "the value of CloakGenerator"
---

With `AIInnerBase=yes`, a computer house that places the structure in its base ranks the free cells by their distance from the house's start location, nearest first, as it does for a [`CloakGenerator=yes`](/keys/cloakgenerator/) structure, rather than by the order that serves other structures. The search then goes outward from the best cell, and a structure that does not fit near the start takes a cell further out. `no` makes the structure follow the ordinary ranking, even when it is a cloak generator.

```ini title="rulesmd.ini"
[GATECH] ; example BuildingType
AIInnerBase=yes
```
