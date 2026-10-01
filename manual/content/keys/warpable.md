---
key: Warpable
summary: "Lets temporal weapons freeze and erase this object."
see_also: [Temporal, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: "yes"
---

A [temporal](/systems/temporal-weapons/) weapon's shot does nothing to an object of a type set to `no`.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
Warpable=no
```
