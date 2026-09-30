---
title: Load the build clock from the side's archives
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: fixed
credit: [Lucas]
---

Starting production no longer crashes the game with Yuri's Revenge's data. The build clock, `GCLOCK2.SHP`, is now loaded with the player's side, where Yuri's Revenge keeps it, and superweapon cameos use the same clock, since there is no `RCLOCK2.SHP`.
