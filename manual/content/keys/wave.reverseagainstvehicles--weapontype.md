---
key: Wave.ReverseAgainstVehicles
scope: weapontype
summary: "Draws the wave from a vehicle back to the firer."
see_also: ["Wave.ReverseAgainstBuildings", "Wave.ReverseAgainstInfantry", "Wave.ReverseAgainstAircraft", "Wave.ReverseAgainstOthers", IsMagBeam]
when_omitted:
  kind: value
  value: "no, except yes for a weapon with IsMagBeam=yes"
---

A sonic or magnetron wave aimed at a vehicle is drawn from the target to the firer, so it appears to pull instead of push. A weapon with `IsMagBeam=yes` draws the wave against a vehicle this way unless the key says `no`. The damage the wave does is not changed.

```ini title="rulesmd.ini"
[MyWave] ; example WeaponType
IsMagBeam=yes
Wave.ReverseAgainstVehicles=yes
```
