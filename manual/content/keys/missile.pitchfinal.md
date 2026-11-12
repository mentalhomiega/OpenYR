---
key: Missile.PitchFinal
summary: "The pitch the missile reaches when the tilt ends and holds as it climbs, from 0.0 to 1.0."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0.0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). The pitch the missile reaches when the tilt ends and holds as it climbs, from 0.0 to 1.0. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.PitchFinal=0.0
```
