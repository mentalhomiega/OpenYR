---
title: Dig in Yuri before his psychic wave fires
category: fix
release: 0.2.0
targets:
- type: key
  id: AreaFire
  effect: changed
  scope: weapontype
- type: key
  id: Deployer
  effect: changed
  scope: infantrytype
credit:
- MentalHomiega
---

A `Deployer=yes` soldier whose `DeployFireWeapon` is `AreaFire=yes`, such as Yuri Clone and Yuri Prime, now digs in when it is ordered to deploy, as Yuri's Revenge does, and fires its psychic wave at its own cell from there. Before, it fired on the spot and never entered a dug-in state, so its `UndeployDelay` had no effect. That delay now packs it up when it ends. The Desolator digs in without aiming its weapon at its own cell on the order.
