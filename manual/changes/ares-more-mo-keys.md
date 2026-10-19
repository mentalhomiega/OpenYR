---
title: Read more Ares keys that Mental Omega uses
category: feature
release: 0.2.0
targets:
- type: key
  id: NoAmmoWeapon
  effect: added
- type: key
  id: NoAmmoAmount
  effect: added
- type: key
  id: InitialPayload.Types
  effect: added
- type: key
  id: InitialPayload.Nums
  effect: added
- type: key
  id: NoManualEnter
  effect: added
- type: key
  id: NoManualUnload
  effect: added
- type: key
  id: Passengers.Allowed
  effect: added
- type: key
  id: Passengers.Disallowed
  effect: added
- type: key
  id: Passengers.BySize
  effect: added
- type: key
  id: FallRate.Parachute
  effect: added
- type: key
  id: FallRate.NoParachute
  effect: added
- type: key
  id: FallRate.ParachuteMax
  effect: added
- type: key
  id: FallRate.NoParachuteMax
  effect: added
- type: key
  id: CivilianEnemy
  effect: added
- type: key
  id: OmniCrusher.Aggressive
  effect: added
- type: key
  id: MassSelectable
  effect: added
- type: key
  id: FakeOf
  effect: added
- type: key
  id: EnemyUIName
  effect: added
- type: key
  id: SelfHealing.Rate
  effect: added
- type: key
  id: Experience.FromPassengers
  effect: added
- type: key
  id: Experience.PromotePassengers
  effect: added
- type: key
  id: Experience.PassengerModifier
  effect: added
- type: key
  id: Experience.FromAirstrike
  effect: added
- type: key
  id: Experience.AirstrikeModifier
  effect: added
- type: key
  id: Experience.MindControlSelfModifier
  effect: added
- type: key
  id: Experience.MindControlVictimModifier
  effect: added
- type: key
  id: Experience.SpawnOwnerModifier
  effect: added
- type: key
  id: Experience.SpawnModifier
  effect: added
- type: key
  id: EngineerRepairable
  effect: added
- type: key
  id: UnitSell
  effect: added
- type: key
  id: IsPassable
  effect: added
- type: key
  id: Cursor.Attack
  effect: added
- type: key
  id: Cursor.AttackOutOfRange
  effect: added
- type: key
  id: Beam.Duration
  effect: added
- type: key
  id: Beam.Amplitude
  effect: added
- type: key
  id: LaserThickness
  effect: added
- type: key
  id: Wave.Color
  effect: added
- type: key
  id: Wave.Intensity
  effect: added
- type: key
  id: Wave.IsBigLaser
  effect: added
- type: key
  id: Wave.ReverseAgainstVehicles
  effect: added
- type: key
  id: Wave.ReverseAgainstBuildings
  effect: added
- type: key
  id: Wave.ReverseAgainstInfantry
  effect: added
- type: key
  id: Wave.ReverseAgainstAircraft
  effect: added
- type: key
  id: Wave.ReverseAgainstOthers
  effect: added
- type: key
  id: MakeInfantryOwner
  effect: added
- type: key
  id: Palette
  effect: added
credit:
- MentalHomiega
---

A type can fire only one weapon when its ammunition runs low (`NoAmmoWeapon=`), start with passengers (`InitialPayload.Types=`), refuse passenger types (`Passengers.Allowed=`) and hide its enter and unload cursors (`NoManualEnter=`, `NoManualUnload=`). `Experience.` keys decide who earns the experience of a kill made by a passenger, an air strike, a mind-controlled object or a spawned craft. `IsPassable=` lets vehicles and soldiers cross a structure, and `Palette=` draws an object type or a particle through a palette file. Weapons gain `Wave.` colour and direction keys, `Beam.Duration=`, `Beam.Amplitude=`, `LaserThickness=` and attack cursors, and `MakeInfantryOwner=` gives the `MakeInfantry=` soldier of an animation to another house. The keys follow the Ares documentation, and a type or weapon that sets none of them behaves as before.
