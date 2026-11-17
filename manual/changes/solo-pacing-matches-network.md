---
title: Hold every game speed to a frame rate
category: feature
release: 0.2.0
targets:
- type: key
  id: GameSpeed
  effect: changed
- type: key
  id: SimulateWhileUnfocused
  effect: changed
- type: system
  id: network-synchronization
  effect: changed
credit:
- ZivDero
- Rampastring
---

Every network game now holds its rate exactly. Each frame's wait used to be rounded down to whole milliseconds, so a network game at 60 ran at 62.5.

A network game now keeps its full rate in the background, minimized or not, without keeping a processor core busy. It used to wait an extra 10 milliseconds each frame, so a game at 45 frames a second fell to about 31. A solo game kept running by `SimulateWhileUnfocused=yes` under `[Options]` in `RA2MD.INI` keeps the same full rate.
