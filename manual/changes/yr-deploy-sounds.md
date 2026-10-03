---
title: Play deploy and pack-up sounds as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: key
  id: DeploySound
  effect: changed
  scope: buildingtype
- type: key
  id: DeploySound
  effect: added
  scope: unittype
- type: key
  id: UndeploySound
  effect: added
  scope: unittype
- type: key
  id: VoiceDeploy
  effect: added
- type: key
  id: VoiceUndeploy
  effect: added
- type: key
  id: BuildupSound
  effect: added
- type: key
  id: DemandLoadBuildup
  effect: changed
credit: [Lucas]
---

Vehicles now play their `DeploySound` as they deploy, a deploy order is answered with `VoiceDeploy`, and a Yuri refinery plays its sound and voice as it turns back into a Slave Miner instead of at build-up. A structure whose build-up art is loaded on demand now falls back to generic art, so the player can pack up a Yuri refinery by ordering it to move.
