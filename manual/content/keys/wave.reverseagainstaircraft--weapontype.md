---
key: Wave.ReverseAgainstAircraft
scope: weapontype
summary: "Draws the wave from an aircraft back to the firer."
see_also: ["Wave.ReverseAgainstVehicles", "Wave.ReverseAgainstBuildings", "Wave.ReverseAgainstInfantry", "Wave.ReverseAgainstOthers", IsMagBeam]
when_omitted:
  kind: value
  value: "no"
---

A sonic or magnetron wave aimed at an aircraft is drawn from the target to the firer, so it appears to pull instead of push. The damage the wave does is not changed.

```ini title="rulesmd.ini"
[MyWave] ; example WeaponType
IsMagBeam=yes
Wave.ReverseAgainstAircraft=yes
```
