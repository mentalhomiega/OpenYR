---
title: Read SuperGapRadiusInCells
category: feature
release: 0.2.0
targets:
- type: key
  id: SuperGapRadiusInCells
  effect: added
- type: key
  id: ExtraPower
  effect: changed
credit:
- MentalHomiega
---

`SuperGapRadiusInCells` is now read, and its owner can charge a gap generator by clicking it. A charged gap generator shrouds that radius and drains the structure's negative `ExtraPower` more, and clicking it again or losing power discharges it. A negative `ExtraPower` is now read as that drain. Stock rules set it on `GAGAP` with an `ExtraPower` of `-9000`, so the charge needs more than 9000 surplus power and is out of reach in stock play.
