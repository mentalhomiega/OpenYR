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
  id: CMislAcceleration
  effect: added
- type: key
  id: CMislAltitude
  effect: added
- type: key
  id: CMislBodyLength
  effect: added
- type: key
  id: CMislDamage
  effect: added
- type: key
  id: CMislEliteDamage
  effect: added
- type: key
  id: CMislLazyCurve
  effect: added
- type: key
  id: CMislPauseFrames
  effect: added
- type: key
  id: CMislPitchFinal
  effect: added
- type: key
  id: CMislPitchInitial
  effect: added
- type: key
  id: CMislRaiseRate
  effect: added
- type: key
  id: CMislTiltFrames
  effect: added
- type: key
  id: CMislTurnRate
  effect: added
- type: key
  id: CMislType
  effect: added
- type: key
  id: DMislAcceleration
  effect: added
- type: key
  id: DMislAltitude
  effect: added
- type: key
  id: DMislBodyLength
  effect: added
- type: key
  id: DMislDamage
  effect: added
- type: key
  id: DMislEliteDamage
  effect: added
- type: key
  id: DMislLazyCurve
  effect: added
- type: key
  id: DMislPauseFrames
  effect: added
- type: key
  id: DMislPitchFinal
  effect: added
- type: key
  id: DMislPitchInitial
  effect: added
- type: key
  id: DMislRaiseRate
  effect: added
- type: key
  id: DMislTiltFrames
  effect: added
- type: key
  id: DMislTurnRate
  effect: added
- type: key
  id: DMislType
  effect: added
- type: key
  id: Missile.Type
  effect: added
- type: key
  id: V3RocketAcceleration
  effect: added
- type: key
  id: V3RocketAltitude
  effect: added
- type: key
  id: V3RocketBodyLength
  effect: added
- type: key
  id: V3RocketDamage
  effect: added
- type: key
  id: V3RocketEliteDamage
  effect: added
- type: key
  id: V3RocketLazyCurve
  effect: added
- type: key
  id: V3RocketPauseFrames
  effect: added
- type: key
  id: V3RocketPitchFinal
  effect: added
- type: key
  id: V3RocketPitchInitial
  effect: added
- type: key
  id: V3RocketRaiseRate
  effect: added
- type: key
  id: V3RocketTiltFrames
  effect: added
- type: key
  id: V3RocketTurnRate
  effect: added
- type: key
  id: V3RocketType
  effect: added
- type: key
  id: ProjectileRange
  effect: changed
- type: key
  id: Ranged
  effect: changed
credit: [MentalHomiega]
---

`TurretROT=` gives a vehicle turret its own rate of turn, `ReloadAmount=` and `EmptyReloadAmount=` set the shots a vehicle regains per reload, and `ProjectileRange=` ends the flight of a `Ranged=yes` projectile. `Missile.Custom=yes` lets an aircraft type fly as a missile on its own `Missile.` keys. The default of each key leaves the current behavior unchanged. The `DMisl` and `CMisl` defaults in `[General]` now match Yuri's Revenge when a rules file leaves their keys out. The `[General]` keys that begin with `V3Rocket`, `DMisl` and `CMisl` now set how those missiles fly, and `V3RocketType`, `DMislType` and `CMislType` name the aircraft type that flies with each set.
