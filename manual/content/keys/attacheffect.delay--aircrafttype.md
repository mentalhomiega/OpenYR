---
key: AttachEffect.Delay
scope: aircrafttype
label: Own effect repeat delay
see_also: ["system:attach-effects"]
when_omitted:
  kind: value
  value: "0"
  note: "The effect is attached again as soon as it ends."
---

`AttachEffect.Delay` is how many frames after an object's [own effect](/systems/attach-effects/#an-object-types-own-effect) ends, or is removed on entry, it is attached again. A negative value means it is not attached again. The frames count only while the object is on the map and not being warped out by a temporal weapon.

```ini title="rulesmd.ini"
[FV] ; example VehicleType
AttachEffect.Duration=60
AttachEffect.Delay=30 ; 60 frames on, 30 off
AttachEffect.SpeedMultiplier=0.5
```
