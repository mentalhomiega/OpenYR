---
key: CMislPitchInitial
scope: global-rules
summary: "The pitch the CMisl missile starts tilting from, from 0.0 (level) to 1.0 (straight up)."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "1.0"
---

The pitch the CMisl missile starts tilting from, from 0.0 (level) to 1.0 (straight up). This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislPitchInitial=1.0
```
