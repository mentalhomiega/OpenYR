---
title: Pick a new firing spot when the target moves away
category: fix
release: 0.2.0
targets:
- type: key
  id: ApproachTargetResetMultiplier
  effect: added
- type: key
  id: CanRecalcApproachTarget
  effect: added
- type: system
  id: slave-miners
  effect: changed
credit:
- MentalHomiega
---

A unit heading for a spot to shoot from now drops the spot and picks a new one once its target moves farther from it than `ApproachTargetResetMultiplier` weapon ranges, as Yuri's Revenge does. `CanRecalcApproachTarget=no` keeps the unit's spot. A slave returning to its miner is now sent again when the miner has moved away from where the slave was heading.
