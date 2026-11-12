---
key: Missile.LazyCurve
summary: "With yes the missile flies a ballistic arc whose pitch follows its distance to the target; with no it holds its pitch until the flat distance to the target is no more than its height, then dives."
see_also: ["Missile.Custom"]
when_omitted:
  kind: value
  value: "no"
---

An Ares key for an AircraftType with [`Missile.Custom=yes`](/keys/missile.custom/). With `yes` the missile flies a ballistic arc whose pitch follows its distance to the target; with `no` it holds its pitch until the flat distance to the target is no more than its height, then dives. A custom missile never falls back to the V3Rocket, DMisl or CMisl values in `[General]`, so set every `Missile.` key the missile needs.

```ini title="rulesmd.ini"
[MYMISSILE] ; example AircraftType
Missile.Custom=yes
Missile.LazyCurve=no
```
