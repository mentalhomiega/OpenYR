---
key: IsLocomotor
summary: "Makes a warhead lift the vehicle it hits and hand it to the firer to hold."
see_also: [Locomotor, IsMagBeam]
when_omitted:
  kind: value
  value: "no"
---

A warhead with `IsLocomotor=yes` does no damage. Its firer first lets go of any vehicle it already holds, even when this hit lifts nothing, such as a hit on a structure. A firer that is itself held lifts nothing. Otherwise the hit lifts a vehicle only when the vehicle is on the ground, nothing else holds it, and its `Weight` is less than the weapon's `Damage`. The vehicle is lifted with the warhead's [`Locomotor`](/keys/locomotor/#scope-warheadtype).

A held vehicle rises into the air, cannot be selected, takes no orders and cannot fire. Its holder pulls it toward itself to two cells away every 15 frames. A held vehicle's carried aircraft or missiles are lost. While the holder holds a vehicle, its beam fires at no vehicle; it fires at a structure, or at a new target once the beam has gone.

The holder lets go when it attacks something else, is ordered to move or stop, or is destroyed or removed. A vehicle let go in the air is stunned and falls straight down. It takes no orders while it falls, and it is destroyed when it lands. A held vehicle killed in the air falls the same way. A vehicle that falls through the height of a bridge deck lands on the deck, standing on it and crushing the soldiers there. A vehicle let go on the ground gets its own locomotor back at once.

```ini title="rulesmd.ini"
[MyMagnetWH] ; example Warhead
IsLocomotor=yes
Locomotor={92612C46-F71F-11d1-AC9F-006008055BB5}
```
