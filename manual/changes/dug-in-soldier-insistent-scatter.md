---
title: Make a dug-in soldier give way to an insistent scatter
category: fix
release: 0.2.0
targets:
- type: key
  id: Deployer
  effect: changed
  scope: infantrytype
credit:
- MentalHomiega
---

A human player's `Deployer=yes` soldier that is dug in now packs up and moves off when a scatter that insists reaches it, such as the one that clears a cell for an arriving vehicle, as in Yuri's Revenge. Before, it stayed dug in. Any other scatter, such as the one after a hit, still leaves it in place. A soldier that is packing up can now scatter.
