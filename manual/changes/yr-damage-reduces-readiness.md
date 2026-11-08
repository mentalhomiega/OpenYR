---
title: Let damage cost ammunition with DamageReducesReadiness
category: feature
release: 0.2.0
targets:
- type: key
  id: DamageReducesReadiness
  effect: added
- type: key
  id: ReadinessReductionMultiplier
  effect: added
credit:
- MentalHomiega
---

`DamageReducesReadiness` and `ReadinessReductionMultiplier` now make hits take ammunition from an object, as in Yuri's Revenge. We ignored both keys before.
