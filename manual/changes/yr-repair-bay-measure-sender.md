---
title: Measure the docking aircraft at a repair pad
category: fix
release: 0.2.0
targets:
- type: key
  id: NumberOfDocks
  effect: changed
credit: [MentalHomiega]
---

A repair pad with several docks now judges a docking aircraft that holds a dock by its own distance from the pad. Before, it always measured the aircraft on the first dock. A pad with one dock behaves as before.
