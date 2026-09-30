---
key: RequiresStolenAlliedTech
summary: Makes the type buildable only after its house has stolen the first side's technology.
see_also: ["system:production"]
when_omitted:
  kind: value
  value: "no"
---

With `RequiresStolenAlliedTech=yes`, a house may build the type only once it has stolen the first side's technology. Nothing steals technology yet, so a type with the key set is never buildable, for human or computer houses.

```ini title="rulesmd.ini"
[MyCommando] ; example InfantryType
RequiresStolenAlliedTech=yes
```
