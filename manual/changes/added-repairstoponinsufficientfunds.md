---
title: Let structure repair pause while a player cannot pay (RepairStopOnInsufficientFunds)
category: feature
release: 0.2.0
targets:
- type: key
  id: RepairStopOnInsufficientFunds
  effect: added
credit: [MentalHomiega]
---

With `[General]` `RepairStopOnInsufficientFunds=no`, a human player's structure that runs out of money for repair now keeps repair on and carries on once the player can pay, as Ares does. Rules that leave the key out are unchanged.
