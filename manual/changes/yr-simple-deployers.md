---
title: Deploy Siege Choppers where they stand
category: feature
release: 0.2.0
targets:
- type: key
  id: IsSimpleDeployer
  effect: added
- type: key
  id: DeployToLand
  effect: added
- type: key
  id: DeployingAnim
  effect: added
  scope: unittype
- type: key
  id: DeployFire
  effect: added
  scope: unittype
- type: key
  id: DeployFireWeapon
  effect: added
  scope: unittype
- type: key
  id: Shadow
  effect: added
  scope: animtype
credit: [Lucas]
---

The Siege Chopper now lands and deploys into artillery when told to deploy, and packs up again, as in Yuri's Revenge. Animations with `Shadow=yes` now draw their shadow frames as shadows instead of playing them as extra frames.
