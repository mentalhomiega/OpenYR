---
key: CMislPauseFrames
scope: global-rules
summary: "Frames the CMisl missile waits on its launcher before it starts to tilt."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "10"
---

Frames the CMisl missile waits on its launcher before it starts to tilt. This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislPauseFrames=10
```
