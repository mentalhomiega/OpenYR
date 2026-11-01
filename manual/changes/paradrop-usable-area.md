---
title: Keep paratroopers inside the usable area
category: feature
release: 0.2.0
targets:
- type: key
  id: ParadropInUsableArea
  effect: added
- type: system
  id: superweapons
  effect: changed
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A paratrooper now lands only on a cell inside the usable map area, by default. [`ParadropInUsableArea=no`](/keys/paradropinusablearea/) in `[General]` restores the earlier landing, which can place a paratrooper outside that area. Save games now store the setting, so a save from an earlier build is refused.
