---
title: Start a rocket's takeoff animation two frames in, with a Z adjust of -10
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit:
- MentalHomiega
---

A rocket's `V3TAKOFF` animation now starts two frames after its climb begins, with a Z adjust of -10, as gamemd plays it. Before, it started at the climb with no delay and no Z adjust.
