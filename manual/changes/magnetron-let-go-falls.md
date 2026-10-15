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

A vehicle the Magnetron's beam lets go in the air no longer keeps flying. It falls straight down, takes no move or stop orders while it falls, and is destroyed when it lands. Before, it kept its jumpjet, moved about the map like an aircraft and could fire.

A lifted vehicle killed in the air also falls and is destroyed on landing. A beam lifts only a vehicle on the ground that weighs less than the weapon's `Damage`. While the beam holds a vehicle, the Magnetron does not fire at other vehicles.
