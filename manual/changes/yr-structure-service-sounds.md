---
title: Add structure service sounds and ClickRepairable
category: feature
release: 0.2.0
targets:
- type: key
  id: ClickRepairable
  effect: added
- type: key
  id: WorkingSound
  effect: added
- type: key
  id: NotWorkingSound
  effect: added
credit: [Lucas]
---

Structures now play `WorkingSound` and `NotWorkingSound` as they come into and drop out of service, and `ClickRepairable=no` structures, such as most civilian buildings, can no longer be repaired with the repair cursor, as in Yuri's Revenge.
