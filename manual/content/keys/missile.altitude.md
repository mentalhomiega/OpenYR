---
key: Missile.Altitude
summary: "Height above the ground, in leptons, at which the missile stops climbing and starts to cruise."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Height above the ground, in leptons, at which the missile stops climbing and starts to cruise. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.Altitude=0
```
