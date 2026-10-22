---
title: Drop a vehicle the Magnetron lets go instead of letting it fly
category: fix
release: 0.2.0
targets:
- type: key
  id: IsLocomotor
  effect: changed
- type: key
  id: IsMagBeam
  effect: changed
- type: key
  id: FallingDamageMultiplier
  effect: added
- type: key
  id: CurrentStrengthDamage
  effect: added
credit: [MentalHomiega]
---

A vehicle the Magnetron's beam lets go in the air no longer keeps flying. It falls straight down and takes no move or stop orders while it falls. While the beam holds a vehicle, a stop order or a change of the Magnetron's target does not let it go; only a move order does. It starts at the type's `JumpjetCrash` speed in leptons a frame and speeds up by one lepton each frame, so a fall from 500 leptons lands in 28 frames. On landing it crushes what is under it and takes no damage from the landing, unless it lands on a vehicle, a structure or a cell it cannot enter. Then it explodes instead: the Crush and then the C4 warhead burst where it lands, each for its strength times `FallingDamageMultiplier` (default 1.0; `CurrentStrengthDamage` picks its current strength instead of its full strength), and it is removed. A land vehicle that lands on water sinks instead, and the holder is credited with the kill. Before, it kept its jumpjet, moved about the map like an aircraft and could fire.

A lifted vehicle killed in the air is destroyed where it is and does not fall. A lifted vehicle flies itself to the free spot beside the Magnetron, at its own jump-jet speed, and is let go when it gets there. It is not pulled in two cells at a time, and the spot can be over water. A beam lifts only a unit or aircraft on the ground layer that is not already held or under an Iron Curtain, that is not standing on the Weapons Factory it is still linked to, and that weighs less than the weapon's `Damage`. The Magnetron does not fire at a unit that is deploying or undeploying, at a harvester that is unloading, or at a jump-jet vehicle that is moving. While the beam holds a vehicle, the Magnetron does not fire at other vehicles.
