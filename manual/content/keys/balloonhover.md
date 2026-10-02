---
key: BalloonHover
summary: "Keeps a jumpjet type in the air when it stops, and shields it from the psychic dominator."
see_also: [ImmuneToPsionics, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

A jumpjet object whose type sets `BalloonHover=yes` stays in the air. It takes off on its own when it is on the ground, flies to the exact cell it is sent to, over a structure if need be, and hovers there instead of landing. Stopping it leaves it hovering where it is. An ion storm still destroys it, as it does any airborne jumpjet. The Rocketeer, the Floating Disc and the Cosmonaut set it.

The psychic dominator does not take over an object whose type is `BalloonHover=yes`. The object still takes the blast's damage.

```ini title="rulesmd.ini"
[MYHOVERER] ; example VehicleType with the jumpjet locomotor
Locomotor={92612C46-F71F-11d1-AC9F-006008055BB5}
BalloonHover=yes
```
