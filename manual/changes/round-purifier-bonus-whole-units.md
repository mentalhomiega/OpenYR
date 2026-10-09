---
title: Round the ore purifier bonus down to whole units
category: fix
release: 0.2.0
targets:
- type: key
  id: PurifierBonus
  effect: changed
- type: system
  id: tiberium
  effect: changed
credit: [MentalHomiega]
---

Ore purifiers now pay their bonus in whole units. A harvester pass multiplies the purifier count, `PurifierBonus` and the load it hands over, then rounds the result down, and the whole units are paid at the type's price and score. Before, the fraction of the bonus was paid as well, so a pass could pay part of a unit that the original game does not.
