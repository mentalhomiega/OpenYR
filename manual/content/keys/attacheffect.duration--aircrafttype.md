---
key: AttachEffect.Duration
scope: aircrafttype
label: Own effect duration
when_omitted:
  kind: value
  value: "0"
  note: "No effect is attached, and the other AttachEffect keys of the section do nothing."
---

`AttachEffect.Duration` is how many frames the effect an object of this type gives itself lasts. Any value other than `0` gives every object of the type the effect, first after [`AttachEffect.InitialDelay`](/keys/attacheffect.initialdelay/) and again [`AttachEffect.Delay`](/keys/attacheffect.delay/) frames after each time it ends. A negative value lasts until the object is destroyed or the effect is discarded on entry.

```ini title="rulesmd.ini"
[FV] ; example VehicleType
AttachEffect.Duration=-1 ; the effect never ends
AttachEffect.ArmorMultiplier=1.25
```
