---
title: Pay a NeedsEngineer structure's cash only after its owner has changed
category: fix
release: 0.2.0
targets:
- type: key
  id: NeedsEngineer
  effect: changed
- type: key
  id: ProduceCashDelay
  effect: changed
- type: system
  id: produce-cash
  effect: changed
credit:
- MentalHomiega
---

A structure with `NeedsEngineer=yes` pays no `ProduceCashAmount` until its owner has changed at least once, which a capture does. A structure the map places under a player's house now pays nothing, as gamemd's power test for the structure does.

The payment interval also runs through an outage. Before this change a `Powered=yes` structure stopped its interval while it lacked power and resumed it later. Now the interval keeps running, and a payment that falls due during an outage is skipped.
