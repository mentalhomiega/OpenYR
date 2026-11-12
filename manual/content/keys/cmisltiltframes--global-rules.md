---
key: CMislTiltFrames
scope: global-rules
summary: "Frames the CMisl missile takes to tilt from CMislPitchInitial to CMislPitchFinal before it climbs."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "30"
---

Frames the CMisl missile takes to tilt from `CMislPitchInitial` to `CMislPitchFinal` before it climbs. This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislTiltFrames=30
```
