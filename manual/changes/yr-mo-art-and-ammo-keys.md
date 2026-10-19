---
title: Read the reload, pip, line trail, ambient sound and cache keys of Yuri's Revenge
category: feature
release: 0.2.0
targets:
- type: key
  id: Reload
  effect: added
- type: key
  id: EmptyReload
  effect: added
- type: key
  id: ReloadIncrement
  effect: added
- type: key
  id: PipWrap
  effect: added
- type: key
  id: AmbientSound
  effect: added
- type: key
  id: UseLineTrail
  effect: added
- type: key
  id: LineTrailColor
  effect: added
- type: key
  id: LineTrailColorDecrement
  effect: added
- type: key
  id: LineTrailColorOverride
  effect: added
- type: key
  id: FlashSidebarTabFrames
  effect: added
- type: key
  id: DisableVoxelCache
  effect: added
- type: key
  id: DisableShadowCache
  effect: added
- type: key
  id: Bullets
  effect: added
- type: key
  id: UseOwnName
  effect: added
- type: key
  id: NoMovingFire
  effect: changed
- type: key
  id: ManualReload
  effect: changed
- type: system
  id: structure-damage-fires
  effect: changed
credit:
- MentalHomiega
---

The engine now reads these keys as Yuri's Revenge does. A vehicle with an `Ammo` limit regains one shot `Reload` frames after it fires (`EmptyReload` when it has none, plus `ReloadIncrement` for the shots it holds), where before a vehicle with `NoMovingFire=yes` regained a shot after 450 frames and no other vehicle reloaded. `PipWrap` arranges ammunition pips in columns, and the ammunition pip is now frame 13 of the pip shape. `AmbientSound` plays a sound at an object while it is on the map, which gives the stock cow, chimpanzee and brontosaurus their ambient sounds. `UseLineTrail`, `LineTrailColor`, `LineTrailColorDecrement` and `LineTrailColorOverride` draw a fading line behind an object. `FlashSidebarTabFrames` lets a ready superweapon flash its tab in the modern sidebar. `DisableVoxelCache` and `DisableShadowCache` redraw a voxel body or shadow every frame. `Bullets` and `UseOwnName` are read and have no effect here. A structure's damage fires stop at a `DamageFireOffset` of `0,0`.
