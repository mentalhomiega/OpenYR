---
title: Fire DeployFire vehicles at their own cell
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployFire
  effect: changed
credit:
- MentalHomiega
---

A vehicle with `DeployFire=yes` and no `IsSimpleDeployer` now fires at its own cell when the player clicks it while it is the only selected object, as Yuri's Revenge does. It fires its first weapon and never its `DeployFireWeapon`, and it keeps that order until it gets another. Only an `AreaFire=yes` weapon reaches the vehicle's own cell, so a first weapon without that flag does not fire. Before, such a vehicle ignored the key, unless it also had `Deployer=yes`. Then it attacked its own cell and could pick its area weapon from either slot.
