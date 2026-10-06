---
title: Control theft, build time, crashes, promotion sounds and health bars per type
category: feature
release: 0.2.0
targets:
- type: key
  id: VehicleThief.Allowed
  effect: added
- type: key
  id: BuildTime.MultipleFactory
  effect: added
- type: key
  id: Crashable
  effect: added
- type: key
  id: Promote.VeteranSound
  effect: added
- type: key
  id: Promote.EliteSound
  effect: added
- type: key
  id: HealthBar.Hide
  effect: added
credit:
- MentalHomiega
---

In rulesmd.ini, an object type can refuse theft by vehicle thieves with `VehicleThief.Allowed=no`, replace `[General] MultipleFactory` with its own `BuildTime.MultipleFactory`, and hide its health bar with `HealthBar.Hide=yes`. An aircraft type with `Crashable=no` is destroyed where it flies instead of falling. `Promote.VeteranSound` and `Promote.EliteSound` replace the promotion sounds for one type. The keys follow the Ares and Phobos documentation.
