---
title: Match gamemd for a soldier on the hunt mission and a dug-in soldier on the attack mission
category: fix
release: 0.2.0
targets:
- type: key
  id: Infiltrate
  effect: changed
- type: key
  id: Deployer
  scope: infantrytype
  effect: changed
credit:
- MentalHomiega
---

A computer's soldier on the hunt mission that has a structure as its target now goes to it on the Capture mission when it has [`Infiltrate=yes`](/keys/infiltrate/), and enters it when it has `Occupier=yes` or `Assaulter=yes` and can garrison it. Before, it only walked up and fired, and a Tanya or Navy SEAL went to plant C4 instead. A computer's soldier with the C4 ability but no `Infiltrate=yes` now heads for any structure on the Sabotage mission, not only one that takes C4.

A computer's dug-in soldier that is put on the hunt mission with no target now packs up first. A computer's `Deployer=yes` soldier now also digs in on the area guard mission, which is the idle mission of most of its soldiers, not only on guard.

A human player's dug-in soldier that is ordered to attack a target out of reach of its deploy weapon now drops the order and stands guard, or takes the greatest threat in reach instead. Before, it kept the order and never fired.
