---
title: Dig in a computer's guarding soldier after its AI delay
category: feature
release: 0.2.0
targets:
- type: key
  id: AIAutoDeployFrameDelay
  effect: added
- type: key
  id: Deployer
  effect: changed
  scope: infantrytype
credit:
- MentalHomiega
---

A computer player's `Deployer=yes` soldier with `DeployFire=yes` and a negative `UndeployDelay` now digs in while it guards, once more than `AIAutoDeployFrameDelay` frames have passed since its guard mission began, as Yuri's Revenge does. A dug-in soldier with `DeployFire=yes` and a negative `UndeployDelay` now takes its guard target from the targets within its `DeployFireWeapon` reach and drops a target that leaves that reach, for human players' soldiers too. Saves made by earlier builds no longer load, because the rules and the mission state now save more fields.
