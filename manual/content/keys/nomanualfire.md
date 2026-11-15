---
key: NoManualFire
summary: With yes, the player cannot give an object of this type a target.
see_also: [AttackCursorOnFriendlies, CanPassiveAquire]
when_omitted:
  kind: value
  value: "no"
---

A `NoManualFire=yes` object shows no attack cursor over an enemy or on the ground, and a click gives it no attack order, as for an unarmed object. It still picks targets on its own and retaliates, within the limits of [`CanPassiveAquire`](/keys/canpassiveaquire/). The orders a computer player gives are not affected.

A soldier that can capture or infiltrate still gets its capture cursor over a structure.

```ini title="rulesmd.ini"
[MYDRONE] ; example VehicleType
NoManualFire=yes
```
