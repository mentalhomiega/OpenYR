---
key: Missile.PauseFrames
summary: "Frames the missile waits on its launcher before it starts to tilt."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Frames the missile waits on its launcher before it starts to tilt. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.PauseFrames=0
```
