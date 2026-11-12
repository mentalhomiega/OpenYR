---
key: Missile.PitchInitial
summary: "The pitch the missile starts tilting from, from 0.0 (level) to 1.0 (straight up)."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0.0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). The pitch the missile starts tilting from, from 0.0 (level) to 1.0 (straight up). A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.PitchInitial=0.0
```
