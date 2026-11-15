---
key: AirRate
scope: aircrafttype
summary: "The frames between animation steps while the object is off the ground."
see_also: [WalkRate, IdleRate]
when_omitted:
  kind: value
  value: "0, so the walk and idle rates apply in the air as well"
---

With `AirRate=` above 0, an object that is above the ground steps its animation once every that many frames, in place of [`WalkRate=`](/keys/walkrate/) and [`IdleRate=`](/keys/idlerate/). It applies whether the object is moving or hovering, and a unit that is taking off or landing counts as above the ground from its first lepton of height. The animation is the one those rates pace, such as the rotor frames of a helicopter.

```ini title="rulesmd.ini"
[MYCOPTER] ; example AircraftType
WalkRate=4
AirRate=1
```
