---
title: Play V3TAKOFF when a CMisl missile leaves its launcher
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit:
- MentalHomiega
---

A CMisl missile now plays the `V3TAKOFF` animation at its launch point as it leaves the launcher, two frames after it appears. It still plays the animation again when its climb starts. Before, it played only the climb animation.
