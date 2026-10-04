---
title: Keep mind-controlled units out of transports and structures
category: fix
release: 0.2.0
targets:
- type: system
  id: mind-control
  effect: changed
- type: system
  id: transports
  effect: changed
- type: system
  id: garrisons
  effect: changed
- type: system
  id: tank-bunkers
  effect: changed
- type: system
  id: capture
  effect: changed
- type: key
  id: Hospital
  effect: changed
- type: key
  id: Armory
  effect: changed
credit: [MentalHomiega]
---

An infantryman or vehicle under mind control could board a transport, garrison a structure, enter a tank bunker, hospital or armory, and enter a structure to repair, capture or infiltrate it. It is now refused, as in Yuri's Revenge. Grinders and structures such as the Bio Reactor still take it in.
