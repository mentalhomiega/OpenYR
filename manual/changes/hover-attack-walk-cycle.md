---
title: Keep a hovering attacker's walk cycle running while it holds a target
category: fix
release: 0.2.0
targets:
- type: key
  id: HoverAttack
  effect: changed
- type: key
  id: DeployToLand
  effect: changed
credit: [MentalHomiega]
---

A `HoverAttack=yes` unit that stands still on a target, and is not deployed, now steps its walk cycle on each walk-rate frame, as it does while moving. Before, its walk cycle froze while it stood still on a target. A `DeployToLand` unit in the air now steps its walk cycle every frame while it stands still; before, it froze.
