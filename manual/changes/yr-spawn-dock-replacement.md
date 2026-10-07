---
title: Keep carried aircraft that dock to rearm
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
- type: key
  id: SpawnReloadRate
  effect: changed
credit: [MentalHomiega]
---

A carried aircraft that docks with its carrier now rearms for `SpawnReloadRate` frames and can launch again. Before, the carrier counted it as lost, the aircraft never came back out, and its place stayed empty for good.
