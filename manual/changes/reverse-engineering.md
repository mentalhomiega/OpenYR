---
title: Reverse engineer ground-up units
category: feature
release: 0.2.0
targets:
- type: key
  id: ReverseEngineersVictims
  effect: added
- type: key
  id: CanBeReversed
  effect: added
- type: key
  id: ReversedAs
  effect: added
credit:
- MentalHomiega
---

In rulesmd.ini, a structure with `Grinding=yes` and `ReverseEngineersVictims=yes` teaches its owner to build the type of each infantryman or vehicle it grinds up, without prerequisites. `ReversedAs` on the victim names a different type to learn, and `CanBeReversed=no` teaches nothing. The keys follow the Ares documentation.
