---
title: Keep poison gas clouds from drifting as far
category: fix
release: 0.2.0
targets:
- type: system
  id: particle-systems
  effect: changed
- type: key
  id: Normalized
  scope: particletype
  effect: changed
credit:
- MentalHomiega
---

A gas particle now wanders at most 2 leptons a frame to either side, as in Yuri's Revenge. Before, it could wander 5, so clouds spread wider than they should. A `Normalized=yes` flame now works out its pace from whole-lepton speeds, as gamemd does, so a flame fired at a diagonal target can step through its animation one frame slower per state than before.
