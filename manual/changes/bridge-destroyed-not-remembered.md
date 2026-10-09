---
title: Test a bridge destruction again on every offer
category: fix
release: 0.2.0
targets:
- type: event
  id: TEVENT_BRIDGE_DESTROYED
  effect: changed
credit: [MentalHomiega]
---

A persistent tag no longer remembers a bridge destruction once it is satisfied. Each later offer to the tag tests the event again, as in Yuri's Revenge. Before, the event stayed satisfied, so a trigger waiting only on it fired on every later offer.
