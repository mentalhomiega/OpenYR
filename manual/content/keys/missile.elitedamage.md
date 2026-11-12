---
key: Missile.EliteDamage
summary: "Damage dealt where the missile explodes when its launcher is elite."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "0"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). Damage dealt where the missile explodes when its launcher is elite. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.EliteDamage=0
```
