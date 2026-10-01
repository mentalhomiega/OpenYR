---
title: Lower prices with factory plants and country cost factors
category: feature
release: 0.2.0
targets:
- type: key
  id: CostInfantryMult
  effect: added
- type: key
  id: CostUnitsMult
  effect: added
- type: key
  id: CostAircraftMult
  effect: added
- type: key
  id: CostBuildingsMult
  effect: added
- type: key
  id: CostDefensesMult
  effect: added
- type: key
  id: FactoryPlant
  effect: added
- type: key
  id: InfantryCostBonus
  effect: added
- type: key
  id: UnitsCostBonus
  effect: added
- type: key
  id: AircraftCostBonus
  effect: added
- type: key
  id: BuildingsCostBonus
  effect: added
- type: key
  id: DefensesCostBonus
  effect: added
credit: [Lucas]
---

A `FactoryPlant=yes` structure, such as the Industrial Plant, now lowers its owner's prices by its `CostBonus` factors, and a country's `Cost...Mult` keys scale the prices it pays for each kind of object, as in Yuri's Revenge.
