---
title: Stop computer units from scanning for targets on a move order
category: fix
release: 0.2.0
targets:
- type: system
  id: target-selection
  effect: changed
- type: key
  id: Aggressive
  effect: changed
- type: key
  id: CanPassiveAquire
  effect: changed
credit:
- MentalHomiega
---

A computer-owned unit on a move order used to pick up a target in its weapon's range, even when its type has no [`OpportunityFire`](/keys/opportunityfire/) setting, and then carry that target along its route. Now such a unit keeps its route with no target. The move order's scan survives only for an armed object whose type sets `OpportunityFire=yes`, and for a computer's member of an [aggressive](/keys/aggressive/) team that is not a suicide team. A unit still turns on whatever attacks it.
