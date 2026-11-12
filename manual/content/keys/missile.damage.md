---
key: Missile.Damage
summary: "Damage dealt where the missile explodes when its launcher is not elite."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Damage dealt where the missile explodes when its launcher is not elite. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.Damage=0
```
