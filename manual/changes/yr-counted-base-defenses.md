---
title: Plan computer bases with Yuri's Revenge defense counts
category: feature
release: 0.2.0
targets:
- type: key
  id: AlliedBaseDefenseCounts
  effect: added
- type: key
  id: SovietBaseDefenseCounts
  effect: added
- type: key
  id: ThirdBaseDefenseCounts
  effect: added
- type: key
  id: AIExtraRefineries
  effect: added
- type: key
  id: AISlaveMinerNumber
  effect: added
credit: [MentalHomiega]
---

A computer house whose side has a base defense count list now plans its base as Yuri's Revenge does: a fixed number of defenses for its difficulty, spread at random through the plan, extra refineries from `AIExtraRefineries`, no extra helipads and no wall. Sides without a list keep the cost-based plan.
