---
title: Refuse plain move orders to a fully deployed Siege Chopper
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A Siege Chopper in ground mode no longer accepts a plain move order: the order is dropped and the unit stays deployed. It moves again after it packs up. Before, a move order given through the order layer moved a deployed chopper; clicks were already refused. Attack-move orders are unchanged.
