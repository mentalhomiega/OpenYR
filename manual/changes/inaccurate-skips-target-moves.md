---
title: Keep an inaccurate projectile's blast where it goes off
category: fix
release: 0.2.0
targets:
- type: key
  id: Inaccurate
  effect: changed
credit: [MentalHomiega]
---

An `Inaccurate=yes` projectile no longer moves its blast onto a target within 42 leptons, or within 128 leptons of an airborne target, as in Yuri's Revenge. Before, only the 32-lepton move was skipped.
