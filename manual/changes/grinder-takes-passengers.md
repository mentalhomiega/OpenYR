---
title: Grind a vehicle's passengers with it, and their passengers too
category: fix
release: 0.2.0
targets:
- type: key
  id: Grinding
  effect: changed
credit:
- MentalHomiega
---

A vehicle that a `Grinding=yes` structure takes in now takes its passengers, and any passenger inside one of them, with it. The owner is paid each one's refund and each is removed from the game. Before, the refunds of a vehicle's passengers were paid, but the passengers stayed attached to the removed vehicle, and a passenger's own passengers were not refunded.
