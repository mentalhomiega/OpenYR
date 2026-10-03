---
title: Set barrels off one after another
category: feature
release: 0.2.0
targets:
- type: key
  id: CausesDelayKill
  effect: added
- type: key
  id: DelayKillFrames
  effect: added
- type: key
  id: DelayKillAtMax
  effect: added
- type: key
  id: EligibleForDelayKill
  effect: added
credit: [MentalHomiega]
---

A `CausesDelayKill` warhead, such as an exploding barrel's, now sets a fuse on nearby `EligibleForDelayKill` structures, which go off one after another, the farther ones later, as in Yuri's Revenge.
