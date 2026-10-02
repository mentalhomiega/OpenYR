---
key: IsLocomotor
summary: "Makes a warhead lift the vehicle it hits and hand it to the firer to hold."
see_also: [Locomotor, IsMagBeam]
when_omitted:
  kind: value
  value: "no"
---

A warhead with `IsLocomotor=yes` does no damage. When it hits a vehicle that nothing else holds, fired by something that is not itself held, the firer lets go of any vehicle it already holds and lifts this one with the warhead's [`Locomotor`](/keys/locomotor/#scope-warheadtype). A held vehicle rises into the air, cannot be selected, takes no orders and cannot fire, and its holder pulls it toward itself to two cells away every 15 frames. A held vehicle's carried aircraft or missiles are lost.

The holder lets go when it attacks something else, is ordered to move, or is destroyed or removed. A vehicle let go in the air falls and is destroyed when it lands; one let go on the ground gets its own locomotor back.

```ini title="rulesmd.ini"
[MyMagnetWH] ; example Warhead
IsLocomotor=yes
Locomotor={92612C46-F71F-11d1-AC9F-006008055BB5}
```
