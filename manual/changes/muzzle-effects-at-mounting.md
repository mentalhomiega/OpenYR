---
title: Start firing animations and electric bolts at the mounting
category: fix
release: 0.2.0
targets:
- type: system
  id: firing-geometry
  effect: changed
- type: key
  id: IsElectricBolt
  effect: changed
credit:
- MentalHomiega
---

A firing animation and an electric bolt now start at the mounting, where the projectile is created, instead of at the barrel tip. A vehicle's first bolt follows the mounting while it lasts. gamemd's `TechnoClass::Fire` (0x6FDD50) starts the firing animation at the point from `TechnoClass::GetFLH` (0x6F3AD0). `UnitClass::FireElectricBolt` (0x6FD570) and `EBolt::GetSourceCoords` (0x4C2B40) take the bolt's start from the same point. No gamemd code path we found reads `BarrelLength`, so the barrel length no longer moves the animation or the bolt. The report sound already started at the mounting.
