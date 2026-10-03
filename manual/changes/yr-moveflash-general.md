---
title: Read MoveFlash from [General]
category: fix
release: 0.2.0
targets:
- type: key
  id: MoveFlash
  effect: changed
credit: [MentalHomiega]
---

`MoveFlash` is now read from `[General]`, where Yuri's Revenge keeps it. It was read from `[AudioVisual]`, so it stayed unset and the player's first move click crashed the game. A missing `MoveFlash` now plays no marker instead of crashing.
