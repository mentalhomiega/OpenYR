---
title: Detonate missiles at their nose
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

A missile that flies a lazy curve, or dives, now explodes when its nose is at the target's height, and the blast is at the nose. `BodyLength` puts the nose ahead of the missile's centre along its facing and nose angle. A V3 rocket aimed at a building on high ground, such as a Yuri statue on a cliff, no longer explodes just after it leaves the launcher when the target is less than about nine levels above the launcher with the stock `V3Rocket` settings. A missile under a bridge also explodes when its nose is within a bridge's height above a target on the ground below. Multiplayer games with an earlier build diverge once one of these missiles detonates.
