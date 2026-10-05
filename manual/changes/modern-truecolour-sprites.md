---
title: Draw PNG sprite sheets in place of SHP frames
category: feature
release: 0.2.0
targets:
- type: format
  id: shp
  effect: changed
credit: [MentalHomiega]
---

A PNG sprite sheet named after an SHP file, such as `GGWEAP.png` for `GGWEAP.SHP`, now draws in place of that SHP's frames on 16-bit surfaces. An optional mask, `GGWEAP_hc.png`, marks the pixels drawn in the owner's house colour. The SHP must still be present, because its frame count, size and shadow frames stay in use.
