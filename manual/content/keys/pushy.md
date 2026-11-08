---
key: Pushy
summary: Keeps an object of this type taking click orders and moving while a direct rocker link holds it.
see_also: [DirectRocker, FallBackCoefficient]
when_omitted:
  kind: value
  value: "no"
---

A vehicle, infantry or aircraft of this type that a [`DirectRocker`](/keys/directrocker/) link holds keeps taking click orders and keeps moving. Without this key, a linked vehicle, infantry or aircraft takes no click orders and does not move while the link stands.

A linked vehicle cannot fire, whether or not its type is `Pushy`. A `Pushy` infantry cannot attack a vehicle that is linked to a different object.

Set it in the type's own section. The example below is for the stock Rhino tank.

```ini title="rules.ini"
[HTNK]
Pushy=yes
```
