---
title: Count a Dreadnought or V3 missile as airborne from the start of its climb
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit:
- MentalHomiega
---

A V3 or Dreadnought missile counts as airborne once it starts to climb. Before, it counted only once it was two levels above the ground. Tanya's C4 cursor no longer appears on a missile from the start of its climb. While the missile pauses and tilts on its pad, the cursor still appears.
