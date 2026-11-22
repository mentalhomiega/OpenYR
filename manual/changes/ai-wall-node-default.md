---
title: Computer houses plan no perimeter wall by default
category: fix
release: 0.2.0
targets:
- type: key
  id: AIBuildsWalls
  scope: global-rules
  effect: changed
credit:
- MentalHomiega
---

The global `AIBuildsWalls` in `[General]` now defaults to `no`, as Yuri's Revenge never plans a perimeter wall. A rules file sets `AIBuildsWalls=yes` to get one for a side that has no base defense count list. Sides with a list, which includes every side in the stock rules, were not affected.
