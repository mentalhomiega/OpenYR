---
title: Read more Ares keys that Mental Omega uses, part five
category: feature
release: 0.2.0
targets:
- type: key
  id: CanDrive
  effect: added
- type: key
  id: CanBeDriven
  effect: added
- type: key
  id: VehicleThief.EnterSound
  effect: added
- type: key
  id: SpyPlane.Mission
  effect: added
- type: key
  id: Firestorm.Wall
  effect: added
- type: key
  id: ProduceCashDisplay
  effect: added
- type: key
  id: DisplayCreditsDelay
  effect: added
- type: key
  id: Spotlight.StartHeight
  effect: added
- type: key
  id: Spotlight.Distance
  effect: added
- type: key
  id: AlternateTheaterArt
  effect: added
- type: key
  id: ApplyDamage
  effect: added
- type: key
  id: Convert.Water
  effect: added
- type: key
  id: Convert.Land
  effect: added
credit:
- MentalHomiega
---

A soldier with `CanDrive=yes` can take over a vehicle of a house that `CanBeDriven=` allows, and `VehicleThief.EnterSound=` plays where a soldier takes a vehicle. `SpyPlane.Mission=` sets the mission of the planes a spy plane weapon sends. `Firestorm.Wall=` is a second name for `FirestormWall=`, `ProduceCashDisplay=` and `DisplayCreditsDelay=` show the cash a structure pays as floating text, and `Spotlight.StartHeight=` and `Spotlight.Distance=` shape a structure's spotlight beam. `AlternateTheaterArt=` draws a type from the art section of its theater, `ApplyDamage=` decides whether a weapon's shots carry damage, and `Convert.Water=` and `Convert.Land=` change a vehicle's type as it crosses between water and land. The keys follow the Ares documentation, and a type, weapon or rule that sets none of them behaves as before.
