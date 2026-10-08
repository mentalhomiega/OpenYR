---
title: Pick a packed-up deployer's weapon as any vehicle does
category: fix
release: 0.2.0
targets:
- type: key
  id: IsSimpleDeployer
  effect: changed
credit: [MentalHomiega]
---

A vehicle with `DeployFire=yes` now fires its normal weapons while packed up, and its DeployFireWeapon only while deployed. Before, a packed-up Siege Chopper always fired its first weapon, whatever the target.
