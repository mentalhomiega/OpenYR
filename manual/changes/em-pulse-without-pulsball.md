---
title: Fire an EM pulse cannon when the rules define no PULSBALL animation
category: fix
release: 0.2.0
targets:
- type: system
  id: emp-pulse
  effect: changed
credit:
- MentalHomiega
---

A rule set without a `PULSBALL` animation no longer crashes the game when an EM pulse cannon starts its charge-up. The cannon waits its 32 frames and fires with no animation.
