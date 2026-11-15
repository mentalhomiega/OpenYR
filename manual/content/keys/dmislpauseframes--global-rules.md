---
key: DMislPauseFrames
scope: global-rules
summary: "Frames the Dreadnought missile waits on its launcher before it starts to tilt."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "10"
---

Frames the Dreadnought missile waits on its launcher before it starts to tilt. This is a `[General]` key and applies to the aircraft type named by `DMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
DMislPauseFrames=10
```
