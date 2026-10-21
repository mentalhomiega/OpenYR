---
title: Start sonic, beam, particle and EM pulse effects at the mounting
category: fix
release: 0.2.0
targets:
- type: system
  id: firing-geometry
  effect: changed
- type: system
  id: emp-pulse
  effect: changed
- type: system
  id: transports
  effect: changed
- type: key
  id: IsSonic
  effect: changed
- type: key
  id: IsMagBeam
  effect: changed
- type: key
  id: IsRadBeam
  effect: changed
- type: key
  id: IsRailgun
  effect: changed
- type: key
  id: IsLaser
  effect: changed
- type: key
  id: UseFireParticles
  effect: changed
- type: key
  id: UseSparkParticles
  effect: changed
- type: key
  id: PrimaryFireDualOffset
  effect: changed
- type: key
  id: EMPulseCannon
  effect: changed
- type: key
  id: PBarrelLength
  effect: changed
- type: key
  id: PBarrelThickness
  effect: changed
- type: key
  id: SBarrelLength
  effect: changed
- type: key
  id: SBarrelThickness
  effect: changed
- type: key
  id: ElitePBarrelLength
  effect: changed
- type: key
  id: ElitePBarrelThickness
  effect: changed
- type: key
  id: EliteSBarrelLength
  effect: changed
- type: key
  id: EliteSBarrelThickness
  effect: changed
credit:
- MentalHomiega
---

Sonic and magnetron waves, fire and spark particles, railgun beams, laser beams, radiation beams and an EM pulse cannon's launch animation and sound now start at the mounting, where the projectile is created. They used to start at the barrel tip, so the barrel length and thickness moved them. The barrel keys, `PBarrelLength`, `PBarrelThickness`, `SBarrelLength`, `SBarrelThickness` and their elite variants, now have no effect, and an infantry projectile starts at the mounting too.

An infantry passenger of an open-topped transport fires from its seat on the transport, at the art's `AlternateFLH0` to `AlternateFLH4` offset. Seat 1 is the passenger that boarded last. Before, each passenger fired from its own firing offset on the transport.

A building with `PrimaryFireDualOffset=yes` now launches its projectile from the same point as its effects. A structure with a voxel barrel launches its projectile and its firing animation from the barrel that fired the previous round, as gamemd's `GetFLH` does.
