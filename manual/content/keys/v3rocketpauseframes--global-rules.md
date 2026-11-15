---
key: V3RocketPauseFrames
scope: global-rules
summary: "Frames the V3 rocket waits on its launcher before it starts to tilt."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0"
---

Frames the V3 rocket waits on its launcher before it starts to tilt. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketPauseFrames=0
```
