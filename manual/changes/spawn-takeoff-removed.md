---
title: Remove a spawn that is taking off when its carrier dies
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

When the object that launches spawns is destroyed or removed, a spawn that is still taking off is now removed without damage, as a docked one already was. Before, it took C4 damage and crashed. A spawn in flight that is not a missile now crashes directly, so it no longer shows the destruction explosion and announcement. A missile still moving keeps flying, as before.
