---
title: Show a player's disguised Mirage tank as terrain and shimmer
category: feature
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: changed
credit: [MentalHomiega]
---

A player's own `DisguiseWhenStill=yes` vehicle looks like terrain in frames 8 to 55 of each 256-frame cycle, counted from the frame its terrain type was picked, and like a tank in the other frames. Both looks carry a translucent shimmer in bursts of that cycle. Other vehicles get no shimmer and no terrain window.
