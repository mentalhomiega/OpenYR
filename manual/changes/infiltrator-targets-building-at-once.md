---
title: Send an armed infiltrator to its target structure
category: fix
release: 0.2.0
targets:
- type: key
  id: Infiltrate
  effect: changed
- type: key
  id: Agent
  effect: changed
credit:
- MentalHomiega
---

A soldier with [`Infiltrate=yes`](/keys/infiltrate/) that takes a structure as its target now also gets the structure as its destination when it has no destination and the structure has `Capturable=yes`, or `Spyable=yes` for an agent, whether or not it carries a weapon. Before, only an unarmed soldier or an agent did, so engineers, Tanya, Navy SEALs and Psi Commandos did not start walking at once. `Agent=yes` now implies `Infiltrate=yes`, as `Engineer=yes` and `C4=yes` already did.
