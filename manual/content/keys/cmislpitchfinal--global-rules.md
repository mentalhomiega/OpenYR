---
key: CMislPitchFinal
scope: global-rules
summary: "The pitch the CMisl missile reaches when the tilt ends and holds as it climbs, from 0.0 (level) to 1.0 (straight up)."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "1.0"
---

The pitch the CMisl missile reaches when the tilt ends and holds as it climbs, from 0.0 (level) to 1.0 (straight up). This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislPitchFinal=1.0
```
