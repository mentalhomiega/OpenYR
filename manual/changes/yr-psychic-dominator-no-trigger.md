---
title: Take units over with the psychic dominator without springing the player-enters trigger
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

A unit the psychic dominator takes over now changes owner without springing the "player enters" trigger. Before, the capture sprang it. An infantryman or aircraft that changes owner this way now also springs its destroyed-any trigger events. Every unit the dominator takes over counts as lost for its old house, and as a kill for the dominator's house. A type marked `DontScore=yes` still springs the events but changes no kill or loss count.
