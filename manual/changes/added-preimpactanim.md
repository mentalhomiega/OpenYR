---
title: Play an animation before a warhead's blast with PreImpactAnim
category: feature
release: 0.2.0
targets:
- type: key
  id: PreImpactAnim
  effect: added
credit:
- MentalHomiega
---

`PreImpactAnim=` in a warhead section plays that animation where a projectile with the warhead hits, and the blast goes off when it ends. The `NUKE` warhead still plays `NUKEBALL` when the key is not set.
