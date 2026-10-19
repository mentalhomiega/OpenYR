---
key: LaserThickness
scope: weapontype
summary: "The width in pixels of the laser this weapon draws."
see_also: [IsLaser, IsHouseColor, LaserDuration]
when_omitted:
  kind: value
  value: "1, the stock single line"
---

An `IsLaser=yes` weapon draws its beam with a core this many pixels wide, in the beam's inner colour; the glow lines of the stock beam stay where they are. At the lowest detail level the beam is a single line whatever the value. A beam that blinks is not widened.

```ini title="rulesmd.ini"
[MyLaser] ; example WeaponType
IsLaser=yes
IsHouseColor=yes
LaserThickness=4
```
