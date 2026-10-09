---
title: Take mind-controlled units over without springing the player-enters trigger
category: fix
release: 0.2.0
targets:
- type: system
  id: mind-control
  effect: changed
credit: [MentalHomiega]
---

A unit that a mind control weapon takes over, and a unit that goes back to its house when its firer is destroyed, now changes owner without springing the "player enters" trigger. Before, both sprang it. A structure, aircraft or infantryman that changes owner this way now also springs its destroyed-any trigger events. Each object that changes owner this way counts as a kill for its new house, and as lost for its old house. A structure marked `Insignificant=yes` is not counted as lost. A type marked `DontScore=yes` still springs the events but changes no kill or loss count.
