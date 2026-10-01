---
key: ImmuneToRadiation
summary: "Spares this object radiation damage."
see_also: [Radiation, "system:radiation"]
when_omitted:
  kind: value
  value: "no"
---

An object of this type takes no damage from the [radiation](/systems/radiation/#damage) of its cell, nor from any warhead with `Radiation=yes`.

```ini title="rulesmd.ini"
[MYSOLDIER] ; example InfantryType
ImmuneToRadiation=yes
```
