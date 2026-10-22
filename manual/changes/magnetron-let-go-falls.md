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
credit: [MentalHomiega]
---

A vehicle the Magnetron's beam lets go in the air no longer keeps flying. It falls straight down and takes no move or stop orders while it falls. On landing it crushes what is under it and takes no damage from the landing. Before, it kept its jumpjet, moved about the map like an aircraft and could fire.

A lifted vehicle killed in the air also falls and lands with 1 hit point. A beam lifts only a vehicle on the ground that weighs less than the weapon's `Damage`. While the beam holds a vehicle, the Magnetron does not fire at other vehicles.
