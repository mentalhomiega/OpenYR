---
title: Step a jumpjet's speed down as it closes on its destination
category: fix
release: 0.2.0
targets:
- type: key
  id: JumpjetSpeed
  effect: changed
- type: key
  id: JumpjetTurnRate
  effect: changed
- type: key
  id: CruiseHeight
  effect: changed
credit: [MentalHomiega]
---

A jumpjet now steps its speed down as it closes on its destination, in the steps of the original game's flight code, and a jumpjet still turned well away from its heading slows further. Before, it slowed at one and two cells, and a Kirov ordered to bomb a building stopped about three and a half cells short of it. The Kirov now reaches the building and bombs it.
