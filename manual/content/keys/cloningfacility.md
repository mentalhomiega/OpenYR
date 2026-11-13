---
key: CloningFacility
summary: "Makes the structure release a copy of every vehicle its owner's factories produce."
see_also: [CloningFacilityIgnoresFactories, Cloneable, Cloning, Naval]
when_omitted:
  kind: value
  value: "no"
---

`CloningFacility` is an Ares key. Each time a weapons factory of the owner releases a vehicle it has finished building, every working structure of that owner with `CloningFacility=yes` releases one more of the same type, free of charge. The copy is a new vehicle of the same type, not of the same strength, rank or load.

A structure is skipped when it is the factory that built the vehicle, is out of the world or is dead. A structure that makes objects itself is also skipped while [`CloningFacilityIgnoresFactories`](/keys/cloningfacilityignoresfactories/) is `yes`. The vehicle's type must also set [`Cloneable=yes`](/keys/cloneable/), the default, and its `Naval` setting must equal the structure's: a naval vehicle is copied only by a naval structure and a land vehicle only by a land one. Only vehicles built by a vehicle factory are copied. Infantry and aircraft are not, and neither is a vehicle placed or captured another way.

The copy leaves through the structure's own exit. A copy that cannot get out is discarded and nothing is refunded. Several cloning structures each make a copy.

```ini title="rulesmd.ini"
[GAPOWR]
CloningFacility=yes
```
