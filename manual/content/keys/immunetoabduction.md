---
key: ImmuneToAbduction
summary: "Parsed marker that an Abductor weapon would not take this object aboard; no weapon in the engine abducts yet."
no_effect: true
when_omitted:
  kind: value
  value: "no"
---

The engine has no `Abductor=` weapon key, so no weapon takes an object aboard and this setting changes nothing.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
ImmuneToAbduction=yes
```
