---
title: Skip a Slave Miner's slaves in Next Object and Previous Object
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit:
- MentalHomiega
---

Next Object and Previous Object no longer stop on a slave that a Slave Miner owns, as in gamemd. Before, a press of either key could select such a slave. Box selection and clicks are unchanged.
