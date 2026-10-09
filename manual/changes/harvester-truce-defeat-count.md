---
title: Count harvesters toward a defeat under the harvester truce
category: fix
release: 0.2.0
targets:
- type: key
  id: HarvesterImmune
  effect: changed
- type: key
  id: HarvesterUnit
  effect: changed
credit: [MentalHomiega]
---

With the harvester truce on, a player who had only harvesters left was defeated outside a short game. Those harvesters now count like any other vehicle, so the player stays in the match. A short game uses its own test and is unchanged.
