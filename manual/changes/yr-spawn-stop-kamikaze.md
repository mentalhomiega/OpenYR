---
title: Release a spawner's pre-launch missiles when it is stopped
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

Stopping a spawner, such as a V3 Launcher, Dreadnought or Boomer, releases the missiles that are on its pad and not yet taking off. Each one flies at its target's cell and detonates there, and the spawner keeps its other missiles docked. Before, a stopped spawner still fired every missile on its pad. A missile that has started to take off flies on as before.
