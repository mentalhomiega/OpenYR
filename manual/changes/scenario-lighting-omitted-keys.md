---
title: Give omitted scenario lighting keys Yuri's Revenge's starting values
category: fix
release: 0.2.0
targets:
- type: key
  id: Ground
  effect: changed
- type: key
  id: Level
  effect: changed
- type: key
  id: IonAmbient
  effect: changed
- type: key
  id: IonRed
  effect: changed
- type: key
  id: IonGreen
  effect: changed
- type: key
  id: IonBlue
  effect: changed
- type: key
  id: IonGround
  effect: changed
- type: key
  id: IonLevel
  effect: changed
- type: key
  id: DominatorAmbient
  effect: changed
- type: key
  id: DominatorRed
  effect: changed
- type: key
  id: DominatorGreen
  effect: changed
- type: key
  id: DominatorBlue
  effect: changed
- type: key
  id: DominatorGround
  effect: changed
- type: key
  id: DominatorLevel
  effect: changed
- type: key
  id: DominatorAmbientChangeRate
  effect: changed
- type: system
  id: ion-storms
  effect: changed
credit:
- MentalHomiega
---

A map that omits a `[Lighting]` key now gets the value Yuri's Revenge gives it: `Ground=.05`, `Level=.008`, `IonAmbient=.87`, `IonRed=.3`, `IonGreen=.4`, `IonBlue=.75`, `IonGround=0` and `IonLevel=0`. Before, an omitted `Ground` or `Level` was `0`, and an omitted ion key copied the ordinary key it pairs with. The ion keys and the `Dominator` keys now also add a hundredth before dropping the fraction, as the ordinary keys already did, so `IonRed=.88` is `.88` rather than `.87`, and `DominatorRed=.857` is `.85` rather than `.86`. `DominatorAmbientChangeRate` is now read in thousands of frames, as Yuri's Revenge reads it, so `DominatorAmbientChangeRate=1` waits 1,000 frames between light steps rather than one; the default stays one frame. Apart from these, maps that write every key look the same. A generated random map now has the storm tint `IonRed=.3`, `IonGreen=.4`, `IonBlue=.75` unless `ION.INI` sets another, and, as in Yuri's Revenge, each of its height levels adds 3 thousandths of full light rather than 1.
