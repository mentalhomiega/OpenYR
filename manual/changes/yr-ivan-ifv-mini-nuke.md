---
title: Set off the death weapon when a Suicide weapon destroys its firer
category: fix
release: 0.2.0
targets:
- type: key
  id: Suicide
  scope: weapontype
  effect: changed
- type: key
  id: DeathWeapon
  effect: changed
credit: [MentalHomiega]
---

A vehicle whose current weapon is a `Suicide=yes` weapon now sets off its `DeathWeapon` when it dies, even without `Explodes=yes`. An IFV carrying a Crazy Ivan blows up with the Ivan's mini nuke around it, as in Yuri's Revenge. Before, it blew up with no nuke.
