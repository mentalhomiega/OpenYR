---
title: Scale loading screens to fill the screen's height
category: fix
release: 0.2.0
targets:
- type: system
  id: loading-screens
  effect: changed
credit:
- MentalHomiega
---

The campaign, skirmish and multiplayer loading screens now grow to fill the screen's height at any resolution, with smooth filtering, instead of only at whole-number sizes. A 1920 by 1080 screen shows them at 1.8 times their size rather than 1 time, and a 2560 by 1440 screen at 2.4 times.
