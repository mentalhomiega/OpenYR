---
title: Leave an occupiable structure of a passive country alone to an engineer
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

An engineer that reaches an occupiable structure of a country with `MultiplayPassive=yes` now leaves it alone, as Yuri's Revenge does, even when the structure is `Capturable=yes`. The engineer is still consumed. We used to capture such a structure. The stock rules have no structure that is both occupiable and `Capturable=yes`, so only mods see the change.
