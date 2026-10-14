---
title: Buildings can add their firing offset to PrimaryFirePixelOffset
category: feature
release: 0.2.0
targets:
- type: key
  id: PrimaryFireDualOffset
  effect: added
credit:
- MentalHomiega
---

`PrimaryFireDualOffset=yes` in `art.ini` makes a building's firing point the `PrimaryFireFLH` position plus its `PrimaryFirePixelOffset`, as the stock Prism Tower and Pillbox do. Their projectiles, firing animations and beams now start at that point instead of at the pixel offset alone.
