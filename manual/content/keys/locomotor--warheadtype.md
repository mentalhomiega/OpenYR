---
key: Locomotor
scope: warheadtype
label: 'Locomotor a locomotor warhead imposes'
see_also: [IsLocomotor]
when_omitted:
  kind: value
  value: "{4A582747-9839-11D1-B709-00A024DDAFD1}"
  note: The teleport locomotor, which cannot lift a vehicle, so an IsLocomotor warhead without this key does nothing.
---

The locomotor an [`IsLocomotor=yes`](/keys/islocomotor/) warhead puts on the vehicle it hits. Only the jumpjet locomotor, `{92612C46-F71F-11D1-AC9F-006008055BB5}`, can lift a vehicle; with any other the warhead does nothing.

```ini title="rulesmd.ini"
[MyMagnetWH] ; example Warhead
IsLocomotor=yes
Locomotor={92612C46-F71F-11d1-AC9F-006008055BB5}
```
