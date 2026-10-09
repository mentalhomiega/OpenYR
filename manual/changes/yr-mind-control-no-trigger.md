---
title: Take mind-controlled units over without springing triggers
category: fix
release: 0.2.0
targets:
- type: system
  id: mind-control
  effect: changed
credit: [MentalHomiega]
---

A unit that a mind control weapon takes over, and a unit that goes back to its house when its firer is destroyed, now changes owner without springing a trigger. Before, both the capture and the return sprang the unit's "player enters" trigger. Yuri's Revenge also raises the destroyed-any trigger events of an infantryman, aircraft or structure that changes owner this way. We do not do that yet.
