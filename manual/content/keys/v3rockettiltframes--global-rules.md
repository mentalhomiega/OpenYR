---
key: V3RocketTiltFrames
scope: global-rules
summary: "Frames the V3 rocket takes to tilt from V3RocketPitchInitial to V3RocketPitchFinal before it climbs."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "60"
---

Frames the V3 rocket takes to tilt from `V3RocketPitchInitial` to `V3RocketPitchFinal` before it climbs. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketTiltFrames=60
```
