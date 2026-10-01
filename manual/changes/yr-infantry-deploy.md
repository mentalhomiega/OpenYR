---
title: Deploy soldiers as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: key
  id: Deployer
  effect: added
- type: key
  id: DeployFire
  effect: added
- type: key
  id: DeployFireWeapon
  effect: added
- type: key
  id: DeploySound
  scope: infantrytype
  effect: added
- type: key
  id: UndeploySound
  scope: infantrytype
  effect: added
- type: key
  id: Sequence
  effect: changed
credit: [Lucas]
---

A `Deployer=yes` soldier, such as the GI, now digs in when told to deploy and packs up when told again, and a `DeployFire=yes` soldier fires its `DeployFireWeapon` while dug in. Sequence sections accept every Yuri's Revenge entry name. The `Idle1` and `Idle2` animations now play at a third of their old speed, and `Hover` at half, as in Yuri's Revenge.
