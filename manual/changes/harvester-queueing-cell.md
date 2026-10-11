---
title: Harvesters wait at the QueueingCell of a busy refinery
category: feature
release: 0.2.0
targets:
- type: key
  id: QueueingCell
  effect: added
credit:
- MentalHomiega
---

A harvester that cannot reserve a refinery now waits near the cell `QueueingCell=` sets in `art.ini` instead of near the refinery's top-left cell. The stock Allied and Soviet refineries set `4,1`, so waiting harvesters stop beside the refinery's second row rather than at its corner.
