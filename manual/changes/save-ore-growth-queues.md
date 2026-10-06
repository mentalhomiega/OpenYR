---
title: Keep the order ore grows and spreads in through a save
category: fix
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A saved game now records which cells are waiting to grow or spread ore and when each is due, so a loaded game grows and spreads ore in the same order the saved game would have. Loading used to queue every ore cell afresh, so ore grew in a different order after a load.
