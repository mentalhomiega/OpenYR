---
title: Stop scattering with PreventScatter warheads
category: feature
release: 0.2.0
targets:
- type: key
  id: PreventScatter
  effect: added
credit:
- MentalHomiega
---

`PreventScatter=yes` in a warhead section of `rules.ini` stops the objects the warhead hits from scattering because of the hit, even when `PlayerScatter=yes` or the `SCATTER` ability would make them. The default is `no`.
