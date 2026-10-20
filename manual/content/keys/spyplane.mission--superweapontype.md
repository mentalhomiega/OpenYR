---
key: SpyPlane.Mission
scope: superweapontype
summary: "The mission the planes of a spy plane weapon fly."
see_also: ["SpyPlane.Type", "SpyPlane.Count", "system:superweapons"]
when_omitted:
  kind: value
  value: "Spyplane Approach, the stock photographing flight"
---

A mission name. Each plane the weapon sends gets that mission and the target cell, in place of the spy plane approach, for a plane named by [`SpyPlane.Type=`](/keys/spyplane.type/) and for the stock `SPYP` alike. A name that is not a mission leaves the stock flight.

A plane that holds a target flies Attack when the key says `Attack`, `Guard`, `Move` or `Hunt`. With `Attack`, a bomber type flies an attack run on the cell.

```ini title="rulesmd.ini"
[BomberSpecial] ; example SuperWeaponType
Type=SpyPlane
SpyPlane.Type=TU16
SpyPlane.Count=2
SpyPlane.Mission=Attack
```
