---
title: Kill the passengers of an aircraft or wreck that is removed
category: fix
release: 0.2.0
targets:
- type: system
  id: destruction-and-debris
  effect: changed
credit:
- MentalHomiega
---

An aircraft removed on the ground, or one that is removed without falling because its type sets `Crashable=no`, now kills its passengers. A vehicle wreck kills its passengers when its death counter ends and the wreck is removed. Before, those passengers stayed attached to the removed object and were never removed from the game.
