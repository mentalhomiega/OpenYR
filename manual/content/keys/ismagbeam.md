---
key: IsMagBeam
summary: "Draws a rippling violet beam from the firer to its target."
see_also: [IsLocomotor, IsSonic]
when_omitted:
  kind: value
  value: "no"
---

A weapon with `IsMagBeam=yes` draws a band of rippling violet light between the firer and the object it fires at, when the firer is not already showing one. The beam lasts while the firer keeps attacking that target and then fades. On a vehicle the beam runs from the vehicle back to the firer. It has no effect on the damage.

```ini title="rulesmd.ini"
[MyMagnet] ; example Weapon
IsMagBeam=yes
Warhead=MyMagnetWH
```
