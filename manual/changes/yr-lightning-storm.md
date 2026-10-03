---
title: Call down the lightning storm
category: feature
release: 0.2.0
targets:
- type: key
  id: Type
  scope: superweapontype
  effect: changed
- type: key
  id: LightningStormDuration
  effect: added
- type: key
  id: LightningDeferment
  effect: added
- type: key
  id: LightningDamage
  effect: added
- type: key
  id: LightningWarhead
  effect: added
- type: key
  id: LightningHitDelay
  effect: added
- type: key
  id: LightningScatterDelay
  effect: added
- type: key
  id: LightningCellSpread
  effect: added
- type: key
  id: LightningSeparation
  effect: added
- type: key
  id: LightningPrintText
  effect: added
- type: key
  id: WeatherConClouds
  effect: added
- type: key
  id: WeatherConBolts
  effect: added
- type: key
  id: WeatherConBoltExplosion
  effect: added
- type: key
  id: LightningSounds
  effect: added
- type: key
  id: StormSound
  effect: added
- type: key
  id: MetallicDebris
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

A `Type=LightningStorm` superweapon now darkens the sky over its target, gathers clouds there and strikes with lightning for `LightningStormDuration` frames, and takes away its enemies' radar, as in Yuri's Revenge. Computer houses fire it at the same targets they pick for the ion cannon.
