---
title: Pick the first explosion animation for a healing shot
category: fix
release: 0.2.0
targets:
- type: key
  id: ExplosionSet
  effect: changed
credit:
- MentalHomiega
---

A shot with negative damage, such as a repair or healing weapon, whose warhead lists several `ExplosionSet` animations or whose impact is on water with a `SplashList`, picked an animation from outside the list and could crash the game. It now plays the first animation of the list.
