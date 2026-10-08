---
title: Print the lightning storm's messages
category: feature
release: 0.2.0
targets:
- type: key
  id: LightningPrintText
  effect: changed
- type: key
  id: StormSound
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

While a storm waits to break, we play the EVA warning and print its message each time the frames left are a multiple of 225. When it breaks, we print its message and play `StormSound`. Both follow `LightningPrintText`, as in Yuri's Revenge. A shot the player fires while a storm is active prints that a storm is already active, whatever `LightningPrintText` says.
