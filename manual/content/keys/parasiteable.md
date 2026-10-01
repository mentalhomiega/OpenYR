---
key: Parasiteable
summary: "Lets a parasite get into this object."
see_also: [Parasite, "system:parasites"]
when_omitted:
  kind: value
  value: "yes"
---

A [parasite](/systems/parasites/#getting-in) cannot get into an object of a type set to `no`.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Parasiteable=no
```
