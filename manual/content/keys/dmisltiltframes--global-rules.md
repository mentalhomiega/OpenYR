---
key: DMislTiltFrames
scope: global-rules
summary: "Frames the Dreadnought missile takes to tilt from DMislPitchInitial to DMislPitchFinal before it climbs."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "60"
---

Frames the Dreadnought missile takes to tilt from `DMislPitchInitial` to `DMislPitchFinal` before it climbs. This is a `[General]` key and applies to the aircraft type named by `DMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
DMislTiltFrames=60
```
