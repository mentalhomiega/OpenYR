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

A unit the psychic dominator takes over now changes hands as Yuri's Revenge does, and it springs no trigger. Before, the capture sprang the unit's "player enters" trigger and the destroyed-any triggers, so a map could react to a dominator blast as if the unit had been destroyed.
