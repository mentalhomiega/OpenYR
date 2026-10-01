---
key: OpenToppedWarpDistance
summary: "How far a temporal firer riding an open-topped transport can hold its target, in cells."
see_also: [Temporal, OpenTopped, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: "5"
---

A [temporal](/systems/temporal-weapons/#letting-go) firer riding an open-topped transport lets go of its target once the target is more than this many cells away. Firers outside transports are not limited.

```ini title="rulesmd.ini"
[CombatDamage]
OpenToppedWarpDistance=5
```
