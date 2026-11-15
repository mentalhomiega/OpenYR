---
key: V3RocketPitchInitial
scope: global-rules
summary: "The pitch the V3 rocket starts tilting from, from 0.0 (level) to 1.0 (straight up)."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0.0"
---

The pitch the V3 rocket starts tilting from, from 0.0 (level) to 1.0 (straight up). This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketPitchInitial=0.0
```
