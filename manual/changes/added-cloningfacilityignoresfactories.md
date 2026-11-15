---
title: Keep factories from cloning vehicles (CloningFacilityIgnoresFactories)
category: feature
release: 0.2.0
targets:
- type: key
  id: CloningFacilityIgnoresFactories
  effect: added
credit: [MentalHomiega]
---

A new `[General]` key, `CloningFacilityIgnoresFactories`, defaults to yes: a cloning structure that also builds objects makes no copies. Set it to no to let such a structure copy vehicles too.
