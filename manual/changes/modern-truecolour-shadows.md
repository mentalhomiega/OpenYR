---
title: Draw PNG shadows, build-up art and structure depth
category: feature
release: 0.2.0
targets:
- type: format
  id: shp
  effect: changed
credit: [MentalHomiega]
---

A PNG sprite sheet that holds every frame of its SHP now also draws the shadows: each shadow frame's alpha darkens the ground the way the SHP's shadow does. A sheet with only the first half of the frames keeps the SHP's shadows. Sheets now also replace build-up animations and other art the game loads on demand, and a structure drawn from a PNG hides and reveals the units around it as its SHP does.
