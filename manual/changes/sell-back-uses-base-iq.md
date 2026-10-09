---
title: Sell damaged structures on base IQ, not tech level
category: fix
release: 0.2.0
targets:
- type: key
  id: SellBack
  effect: changed
- type: system
  id: repair
  effect: changed
credit: [MentalHomiega]
---

Before, a computer house sold a badly damaged structure it could not repair once its tech level reached `[IQ] SellBack`. Now the house's base IQ, the map's `IQ=` value, must reach it. The tech level still sets how soon the sale happens. A computer house that a skirmish or multiplayer match sets up has base IQ `0`, so with the default `SellBack=2` it no longer sells damaged structures.
