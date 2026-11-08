---
title: Set a rally point on a repair depot
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit: [MentalHomiega]
---

A repair depot takes a rally point from a plain click on open ground, as a factory or cloning vat does. Before, the click did nothing.

A vehicle that a human player's depot has repaired now drives to the rally point. Without a rally point it leaves for the cell beside the depot, as before. A vehicle owned by an AI player still goes to its own destination when it has one.

This applies to every building with `UnitRepair=yes`, so shipyards also send repaired ships to their rally point.
