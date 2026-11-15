---
key: DMislPitchFinal
scope: global-rules
summary: "The pitch the Dreadnought missile reaches when the tilt ends and holds as it climbs, from 0.0 (level) to 1.0 (straight up)."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "1.0"
---

The pitch the Dreadnought missile reaches when the tilt ends and holds as it climbs, from 0.0 (level) to 1.0 (straight up). This is a `[General]` key and applies to the aircraft type named by `DMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
DMislPitchFinal=1.0
```
