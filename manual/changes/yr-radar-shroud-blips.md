---
title: Keep other houses' blips off the radar under the shroud
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
- type: system
  id: map-visibility
  effect: changed
credit: [MentalHomiega]
---

We no longer draw a blip under the shroud for an object whose house the local player does not control. Before, an enemy, neutral or computer unit that the player sensed drew on the radar over black, shrouded ground. Our own objects still draw in shrouded cells, and every object the radar plots still draws in cells the player has revealed. [The radar test](/systems/cloaking/#on-the-radar) gives the order in which an object is plotted.
