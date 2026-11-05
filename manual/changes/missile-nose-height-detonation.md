---
title: Detonate missiles by their nose height
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

A missile that flies a lazy curve, or dives, now explodes when its nose is at the target's height, where `BodyLength` puts the nose ahead of its centre along its nose angle. A V3 rocket aimed at a building on a plateau higher than its cruise altitude, such as a Yuri statue on a tall cliff, no longer explodes just after it leaves the launcher. Multiplayer games with an earlier build diverge once one of these missiles detonates.
