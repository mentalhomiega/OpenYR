---
key: Wave.ReverseAgainstOthers
scope: weapontype
summary: "Draws the wave from any other target, such as a tree or a bare cell back to the firer."
see_also: ["Wave.ReverseAgainstVehicles", "Wave.ReverseAgainstBuildings", "Wave.ReverseAgainstInfantry", "Wave.ReverseAgainstAircraft", IsMagBeam]
when_omitted:
  kind: value
  value: "no"
---

A sonic or magnetron wave aimed at any other target, such as a tree or a bare cell is drawn from the target to the firer, so it appears to pull instead of push. The damage the wave does is not changed.

```ini title="rulesmd.ini"
[MyWave] ; example WeaponType
IsMagBeam=yes
Wave.ReverseAgainstOthers=yes
```
