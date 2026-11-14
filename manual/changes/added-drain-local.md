---
title: Let a drain take only the drained plant's power (Drain.Local)
category: feature
release: 0.2.0
targets:
- type: key
  id: Drain.Local
  effect: added
- type: key
  id: Drain.Amount
  effect: added
credit: [MentalHomiega]
---

A power drain where the drainer or the drained plant sets `Drain.Local=yes` now takes only that plant's power, limited by `Drain.Amount`, instead of all its owner's power, as Ares does. Types that leave the keys out are unchanged.
