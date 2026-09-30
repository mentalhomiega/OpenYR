---
key: Naval
summary: Marks the type as naval.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "no"
---

A computer house that cannot place a finished `Naval=yes` structure abandons it and refunds its cost, like any structure that cannot be placed. From then on it removes every `Naval=yes` structure from its base plan as it reaches it, instead of trying to build it again. A structure it already owns is not affected.

```ini title="rulesmd.ini"
[MyShipyard] ; example BuildingType
Naval=yes
```

The key is read for every type. It has no other effect yet: naval units still move and place like other units.
