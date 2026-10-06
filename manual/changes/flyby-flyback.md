---
title: Read FlyBy and FlyBack
category: feature
release: 0.2.0
targets:
- type: key
  id: FlyBy
  effect: added
- type: key
  id: FlyBack
  effect: added
credit: [MentalHomiega]
---

`FlyBy=` and `FlyBack=` are now read from an aircraft type. An aircraft that sets either one is no longer removed from the game when it flies off the map. Both default to `no`.

Saved games made by earlier builds are refused, because a type now stores the two flags.
