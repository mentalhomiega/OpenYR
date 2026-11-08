---
title: Refuse move orders to a unit held by a giant squid
category: fix
release: 0.2.0
targets:
- type: system
  id: parasites
  effect: changed
credit:
- MentalHomiega
---

A unit that a giant squid holds takes no move order. The order gives no move voice and makes no move mission, so the unit stays where it is. Attack orders are still accepted. Before, the move order was accepted, and the unit kept a move mission that never moved it.
