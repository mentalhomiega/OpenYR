---
title: Let infantry regain ammunition over time
category: fix
release: 0.2.0
targets:
- type: key
  id: Reload
  effect: changed
- type: key
  id: Ammo
  effect: changed
credit: [MentalHomiega]
---

An infantryman with an [`Ammo`](/keys/ammo/) limit now regains one shot each time its [`Reload`](/keys/reload/) countdown runs out, as Ares does. Yuri's Revenge reloads only vehicles, so before this an infantryman stayed empty until its fear wore off. A `Reload` of `-1` gives a shot back on the next frame.
