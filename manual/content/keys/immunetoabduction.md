---
key: ImmuneToAbduction
summary: "With yes, an Abductor weapon cannot take this object aboard and deals its ordinary damage instead."
see_also: [Abductor]
when_omitted:
  kind: value
  value: "no"
---

An [`Abductor=yes`](/keys/abductor/) weapon that hits an `ImmuneToAbduction=yes` object does not take it aboard. The hit damages the object as any other shot does.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
ImmuneToAbduction=yes
```
