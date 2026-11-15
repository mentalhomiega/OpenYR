---
key: Missile.RaiseRate
summary: "Read from the rules, but nothing in the flight uses it."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0.0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Read from the rules, but nothing in the flight uses it. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.RaiseRate=0.0
```
