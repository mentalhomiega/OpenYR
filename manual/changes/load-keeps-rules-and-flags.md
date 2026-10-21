---
title: Keep rules and state that a load reset to defaults
category: fix
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A loaded game now keeps `MobileFire`, `FireWhileMoving`, `YuriBarracks`, the `AlternateFLH0` to `AlternateFLH4` seat offsets, `AlliesAllowed`, `WaterCrate`, the `VoxelSpecularStrength`, `VoxelShadeScale` and `VoxelSpecularFromView` lighting keys and the two `LetsDoTheTimeWarp` sounds, which it used to reset to their defaults. It also keeps whether a match is a team game, which decides if computer players change alliances, and the house that a gas particle credits its kills to. A trainable type's `AltCameo` shows again after a load once the player has spied on a barracks or war factory. Saves from earlier builds no longer load.
