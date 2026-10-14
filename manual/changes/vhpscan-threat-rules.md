---
title: Apply the VHPScan threat rules to target scans
category: feature
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
credit:
- MentalHomiega
---

A type with `VHPScan=Normal` now weighs each candidate by its estimated health. A candidate whose estimate is below `1` scores half, and one whose estimate is at most half its type's strength scores double. A type with `VHPScan=Strong` passes over a candidate whose estimate is below `1`. Other values, and an unset key, leave scans as before. The rules follow the threat filter of `TechnoClass::GreatestThreat` in gamemd (0x6F7CA0).
