---
title: Skip a Slave Miner's slaves in box selection and the object keys
category: fix
release: 0.2.0
targets:
- type: system
  id: band-selection
  effect: changed
- type: system
  id: slave-miners
  effect: changed
credit:
- MentalHomiega
---

A box selection no longer takes a slave that a Slave Miner owns, as in gamemd. The Next Object and Previous Object keys also pass over such a slave. A click on a slave still selects it.
