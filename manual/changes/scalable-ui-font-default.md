---
title: Draw the UI in the scalable font by default
category: feature
release: 0.2.0
targets:
- type: key
  id: BitmapSystemFont
  effect: changed
credit:
- MentalHomiega
---

[`BitmapSystemFont`](/keys/bitmapsystemfont/#scope-client-settings) now defaults to `no`, so lists, text boxes and tooltips draw in the scalable face. A settings file that sets `BitmapSystemFont` keeps its value. The Display page has a Pixel-perfect UI font switch that selects the bitmap face.
