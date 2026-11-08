---
title: Match gamemd for a soldier's attack on a structure and its own target choice
category: fix
release: 0.2.0
targets:
- type: key
  id: Infiltrate
  effect: changed
- type: key
  id: Engineer
  effect: changed
credit:
- MentalHomiega
---

A soldier of a player that is ordered to attack a structure now attacks it, whether or not it has [`Infiltrate=yes`](/keys/infiltrate/). Before, an engineer, spy, Tanya, Navy SEAL or Psi Commando went to the structure on the Capture mission. A computer's soldier with `Infiltrate=yes`, or with `Occupier=yes` or `Assaulter=yes` that can garrison the structure, still goes to it.

A computer's soldier picks a capturable structure for itself when it has [`Engineer=yes`](/keys/engineer/), armed or not. Before, only an unarmed soldier with `Infiltrate=yes` did, so the stock Engineer, which carries a defuse kit, never did. A player's armed soldier now also picks a structure that has a weapon as its own target, such as a pillbox.
