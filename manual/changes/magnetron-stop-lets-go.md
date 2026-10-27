---
title: Let a vehicle go when its Magnetron is stopped
category: fix
release: 0.2.0
targets:
- type: key
  id: IsMagBeam
  effect: changed
credit: [MentalHomiega]
---

A stop order on a Magnetron that holds a vehicle ends the hold. A vehicle in the air is let go where it is and falls straight down, as it does after a move order. Before, the Magnetron kept the hold, and the vehicle went on flying its lift path to the spot beside the Magnetron. A change of the Magnetron's target still does not let the vehicle go.
