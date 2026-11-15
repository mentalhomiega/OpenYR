---
title: Deal animation damage with the animation's warhead
category: fix
release: 0.2.0
targets:
- type: key
  id: Warhead
  scope: animtype
  effect: changed
credit:
- MentalHomiega
---

An animation that deals damage while it plays now uses its own `Warhead` instead of always `FlameDamage2`, as Ares does, and its owner counts as the attacker, so an `AffectsAllies=no` warhead spares the owner's side. A Mental Omega hero's dust field no longer burns the hero.
