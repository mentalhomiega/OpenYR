---
key: Cloakable.Allowed
summary: "With no, objects of this type never cloak."
see_also: [Cloakable, CloakGenerator]
when_omitted:
  kind: value
  value: "yes"
---

`Cloakable.Allowed` is an Ares key. With `Cloakable.Allowed=no`, an object of this type never starts to cloak: not through its own [`Cloakable=yes`](/keys/cloakable/), not inside a [Cloak Generator](/keys/cloakgenerator/)'s field, and not through a veteran or elite cloak ability. A vehicle of the type that enters the map from its edge also starts uncloaked.

A cloak crate picked up by such an object pays money instead. A cloak crate picked up by another object does not give the cloak to objects of the type that stand within the crate's reach.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Cloakable.Allowed=no
```
