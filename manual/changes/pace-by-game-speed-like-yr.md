---
title: Pace skirmishes and campaigns as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: GameSpeed
  effect: changed
credit:
- MentalHomiega
---

A skirmish now runs at 62.5 frames a second at Faster and at 31.25 at Fast, 20.8 at Medium, 15.6 at Slow, 12.5 at Slower and 10.4 at Slowest, with Fastest unlimited, as Yuri's Revenge does. Before, each setting ran one step faster than its label: Fast gave 45 frames a second and Medium 30.

A campaign mission always runs at 31.25 frames a second, and the game controls dialog has no speed slider during a mission, as in Yuri's Revenge.

A frame that takes longer than its length is no longer made up by shorter frames after it. The game used to run shorter frames to keep its average rate, so a mission with slow frames played faster than its setting between the slow ones. It now plays below its rate during slow frames and never above it.
