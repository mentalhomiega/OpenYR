---
title: Pick a Mirage tank only while it blinks
category: fix
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: changed
- type: key
  id: DisabledDisguiseDetectionPercent
  effect: added
- type: key
  id: DisguiseFakeBlinkTime
  effect: added
credit: [MentalHomiega]
---

A computer house's units now pick a Mirage tank as an automatic target only in the frames after it fires, and only on a roll. Before, they picked it whenever it was in range. The frames come from `DisguiseFakeBlinkTime` on the weapon, and the roll from `DisabledDisguiseDetectionPercent`, which lists one percent per side. A human house's units no longer pick a disguised Mirage tank automatically. The save layout changed, so saves from earlier builds do not load.
