---
title: Match gamemd's detection radar event, limbo radar state and red-health firing turn
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
- type: system
  id: map-visibility
  effect: changed
- type: system
  id: garrisons
  effect: changed
credit:
- MentalHomiega
---

A detection of a cloaked or subterranean object still raises the enemy-sensed radar event, and that event plays no announcer line. Before, a detection played the Tiberian Sun speech line for it when no announcer file was loaded. With the Yuri's Revenge announcer file, the detection lines had no entry and were already silent.

An object in limbo is still never plotted or tracked. The radar update now runs for it as it does in Yuri's Revenge: it keeps the radar position, and it marks the object as discovered when the position is unshrouded. The statement in the 0.1.0 record "Keep limbo objects out of radar tracking" that radar updates reject a limbo object before discovery no longer holds.

A structure with `TechLevel=-1` at `ConditionRed` health resets its firing turn to the first occupant on every frame, whether or not anyone is inside. Before, the reset ran only while occupants were present.
