---
key: Missile.TiltFrames
summary: "Frames the missile takes to tilt from Missile.PitchInitial to Missile.PitchFinal before it climbs."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Frames the missile takes to tilt from `Missile.PitchInitial` to `Missile.PitchFinal` before it climbs. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.TiltFrames=0
```
