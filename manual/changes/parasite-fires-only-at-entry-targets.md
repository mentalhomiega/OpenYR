---
title: Stop parasite weapons firing at targets they cannot enter
category: fix
release: 0.2.0
targets:
- type: system
  id: parasites
  effect: changed
- type: key
  id: Parasite
  effect: changed
credit: [MentalHomiega]
---

A parasite weapon no longer fires at a structure, at a vehicle inside a bunker, or at a victim that already holds a parasite. We match Yuri's Revenge here. Before, the parasite leapt, failed to get in and came back out.
