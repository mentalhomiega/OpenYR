---
key: CMislTurnRate
scope: global-rules
summary: "How much the CMisl missile changes its pitch each frame once it has levelled off, in the same 0.0 to 1.0 units as the pitch keys."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "1.0"
---

How much the CMisl missile changes its pitch each frame once it has levelled off, in the same 0.0 to 1.0 units as the pitch keys. It sets how sharply the missile turns toward its target. This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislTurnRate=1.0
```
