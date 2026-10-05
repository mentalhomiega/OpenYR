---
title: Draw PNG sprites that have no SHP
category: feature
release: 0.2.0
targets:
- type: format
  id: shp
  effect: changed
credit: [MentalHomiega]
---

A PNG sprite sheet whose SHP does not exist now adds that art, so a soldier with `Image=TRUEMAN` can be drawn from `TRUEMAN.png` alone. By default the sheet is one row of square cells, or a single cell when its width is not a multiple of its height, and each frame gets an empty shadow frame. An optional `TRUEMAN.png.ini` sets the cell size and the frame count.
