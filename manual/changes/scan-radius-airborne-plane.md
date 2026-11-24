---
title: Measure the scan radius to an airborne candidate on the ground plane
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
credit:
- MentalHomiega
---

A scan with a radius, such as Guard Area, now measures the distance to an airborne candidate without height. Before, the height difference counted, so an anti-air unit missed an aircraft near the edge of its radius.
