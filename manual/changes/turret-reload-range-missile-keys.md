---
title: Turret rate, reload amount, projectile range and custom missile keys
category: feature
release: 0.2.0
targets:
- type: key
  id: TurretROT
  effect: added
- type: key
  id: ReloadAmount
  effect: added
- type: key
  id: EmptyReloadAmount
  effect: added
- type: key
  id: Missile.Custom
  effect: added
- type: key
  id: Missile.PauseFrames
  effect: added
- type: key
  id: Missile.TiltFrames
  effect: added
- type: key
  id: Missile.PitchInitial
  effect: added
- type: key
  id: Missile.PitchFinal
  effect: added
- type: key
  id: Missile.TurnRate
  effect: added
- type: key
  id: Missile.RaiseRate
  effect: added
- type: key
  id: Missile.Acceleration
  effect: added
- type: key
  id: Missile.Altitude
  effect: added
- type: key
  id: Missile.BodyLength
  effect: added
- type: key
  id: Missile.LazyCurve
  effect: added
- type: key
  id: Missile.Damage
  effect: added
- type: key
  id: Missile.EliteDamage
  effect: added
- type: key
  id: Missile.Warhead
  effect: added
- type: key
  id: Missile.EliteWarhead
  effect: added
- type: key
  id: Missile.Weapon
  effect: added
- type: key
  id: Missile.EliteWeapon
  effect: added
- type: key
  id: Missile.TakeOffAnim
  effect: added
- type: key
  id: Missile.TrailerAnim
  effect: added
- type: key
  id: Missile.TrailerSeparation
  effect: added
- type: key
  id: ProjectileRange
  effect: changed
- type: key
  id: Ranged
  effect: changed
credit: [MentalHomiega]
---

`TurretROT=` gives a vehicle turret its own rate of turn, `ReloadAmount=` and `EmptyReloadAmount=` set the shots a vehicle regains per reload, and `ProjectileRange=` ends the flight of a `Ranged=yes` projectile. `Missile.Custom=yes` lets an aircraft type fly as a missile on its own `Missile.` keys. The default of each key leaves the current behavior unchanged. The `DMisl` and `CMisl` defaults in `[General]` now match Yuri's Revenge when a rules file leaves their keys out.
