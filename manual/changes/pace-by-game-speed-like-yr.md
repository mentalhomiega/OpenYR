---
title: Pace skirmishes and campaigns by the game speed
category: fix
release: 0.2.0
targets:
- type: key
  id: GameSpeed
  effect: changed
credit:
- MentalHomiega
---

A skirmish or campaign mission now runs at 60 frames a second at Fastest, then 45, 30, 20, 15, 12 and 10 at Slowest, as a multiplayer game does. Before, each setting ran one step faster than its label: Fast gave 45 frames a second and Medium 30, and Fastest had no limit. The speed slider is available in a campaign again. [`ClassicGameSpeeds`](/keys/classicgamespeeds/) gives Yuri's Revenge's own single player rates instead.

A frame that takes longer than its length is no longer made up by shorter frames after it. The game used to run shorter frames to keep its average rate, so a mission with slow frames played faster than its setting between the slow ones. It now plays below its rate during slow frames and never above it.
