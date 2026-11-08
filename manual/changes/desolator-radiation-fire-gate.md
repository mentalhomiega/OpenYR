---
title: Desolator fires its radiation only while the cell has faded
category: fix
release: 0.2.0
targets:
- type: system
  id: radiation
  effect: changed
credit:
- MentalHomiega
---

A deployed Desolator now fires its radiation at its own cell only when the radiation there has fallen below a third of its deploy weapon's `RadLevel`, or when no radiation is there. Otherwise it waits 10 to 20 frames and checks again. Before, it fired every reload, which is every 60 frames for the stock Desolator, so each shot added about 440 to the cell's level and the patch lasted many minutes after the Desolator was dug in. While the Desolator is deployed, the level at its cell now cycles between about a third and four thirds of the weapon's `RadLevel`. Once it is gone, the patch fades as usual.
