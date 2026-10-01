---
key: DamageSelf
summary: "Lets an object's own blasts damage it."
see_also: ["system:warheads"]
when_omitted:
  kind: value
  value: "no"
---

A blast credited to an object leaves that object out unless its type is `DamageSelf=yes`. With the key set, the object is hit like anything else in [reach](/systems/warheads/#the-cells-in-reach).

```ini title="rulesmd.ini"
[MYSILO] ; example BuildingType
DamageSelf=yes
```

The key applies to objects found by the cell sweep. A flying object can always be hit by its own blast.
