---
title: Remove a replaced building animation without its end sound
category: fix
release: 0.2.0
targets:
- type: key
  id: StopSound
  scope: animtype
  effect: changed
credit: [MentalHomiega]
---

When a building swaps one of its attached animations for another, such as its damaged form, the old animation is removed without its [`StopSound`](/keys/stopsound/#scope-animtype), as in Yuri's Revenge. Before, the sound played. Other removals still play it.
