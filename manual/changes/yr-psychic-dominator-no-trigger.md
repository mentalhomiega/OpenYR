---
title: Take units over with the psychic dominator without springing triggers
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

A unit the psychic dominator takes over now changes owner without springing a trigger. Before, the capture sprang the unit's "player enters" trigger. Yuri's Revenge also raises the destroyed-any trigger events of an infantryman or aircraft that changes owner this way. We do not do that yet.
