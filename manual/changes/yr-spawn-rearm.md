---
title: Re-arm carried aircraft after they dock
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

Aircraft that dock with their carrier now get their ammunition and strength back after `SpawnReloadRate` and launch again on the next attack. Before, a Hornet stayed docked with no ammunition, so a carrier launched its Hornets once and never again.
