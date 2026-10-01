---
key: LimboLaunch
summary: "Makes a weapon take its firer off the map to ride the shot."
see_also: [Parasite, "system:parasites"]
when_omitted:
  kind: value
  value: "no"
---

A vehicle, soldier or aircraft firing this weapon leaves the map as the shot goes off. Only a [parasite](/systems/parasites/#getting-in) weapon brings the firer back, when the shot lands; with any other warhead the firer stays off the map.

```ini title="rulesmd.ini"
[MyDroneJump] ; example Weapon
LimboLaunch=yes
```
