---
key: CanC4
summary: "Lets airstrikes be aimed at this structure."
see_also: [Airstrike]
when_omitted:
  kind: value
  value: "yes"
---

With `CanC4=no`, a unit with an [`Airstrike`](/keys/airstrike/) second weapon does not choose that weapon against the structure.

```ini title="rulesmd.ini"
[MYBUNKER] ; example BuildingType
CanC4=no
```
