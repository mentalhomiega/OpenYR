---
title: Stop crushers from running over falling, shielded or dug-in soldiers
category: fix
release: 0.2.0
targets:
- type: key
  id: Crusher
  effect: changed
- type: key
  id: Crushable
  effect: changed
- type: key
  id: DeployedCrushable
  effect: added
credit: [MentalHomiega]
---

A vehicle with `Crusher=yes` no longer runs over a soldier that is still falling from a paradrop, an Iron Curtained object, or a deployed soldier whose type sets `DeployedCrushable=no`. Before, the Iron Curtain did not protect a crushable soldier from a crusher that lacked `OmniCrusher`, and a falling soldier could be crushed.
