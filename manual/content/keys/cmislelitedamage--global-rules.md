---
key: CMislEliteDamage
scope: global-rules
summary: "Damage the CMisl missile deals where it explodes when its launcher is elite."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "500"
---

Damage the CMisl missile deals where it explodes when its launcher is elite. This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislEliteDamage=500
```
