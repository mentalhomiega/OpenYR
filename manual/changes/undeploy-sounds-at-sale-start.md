---
title: Play the undeploy sounds when the sale begins
category: fix
release: 0.2.0
targets:
- type: key
  id: DeploySound
  effect: changed
  scope: buildingtype
- type: key
  id: VoiceDeploy
  effect: changed
- type: key
  id: SellSound
  effect: changed
credit:
- MentalHomiega
---

A structure that turns back into its vehicle now plays its `DeploySound` and its owner's `VoiceDeploy` as the sale begins, as Yuri's Revenge does, instead of when the vehicle appears. It no longer plays `SellSound` as well.
