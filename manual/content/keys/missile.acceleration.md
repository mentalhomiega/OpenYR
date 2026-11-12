---
key: Missile.Acceleration
summary: "Speed the missile gains each frame while it climbs and cruises, never past the type's Speed."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0.0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Speed the missile gains each frame while it climbs and cruises, never past the type's `Speed`. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.Acceleration=0.0
```
