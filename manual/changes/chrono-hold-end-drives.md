---
title: Drive after a chrono reinforcement or chronosphere hold ends
category: fix
release: 0.2.0
targets:
- type: key
  id: ChronoDelay
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A unit set down by a chronosphere or by chrono reinforcement now gets its own locomotor back in the frame its hold ends. Before, a team ordered to move on the next frame made a second chrono jump to its destination instead of driving there.
