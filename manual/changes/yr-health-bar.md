---
title: Draw unit health bars as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: key
  id: PixelSelectionBracketDelta
  effect: added
credit: [MentalHomiega]
---

A selected vehicle, aircraft or infantry soldier now shows Yuri's Revenge's health bar: green, yellow or red pips inside the `PIPBRD.SHP` border. Before, the bar used Tiberian Sun's pip frames and drew white stripes. The new `PixelSelectionBracketDelta` moves the border and bar up or down for each type.
