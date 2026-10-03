---
title: Read Yuri's Revenge animation and lobby keys from their sections
category: fix
release: 0.2.0
targets:
- type: key
  id: BarrelDebris
  effect: changed
- type: key
  id: BarrelExplode
  effect: changed
- type: key
  id: BarrelParticle
  effect: changed
- type: key
  id: BridgeExplosions
  effect: changed
- type: key
  id: DropZoneAnim
  effect: changed
- type: key
  id: FlamingInfantry
  effect: changed
- type: key
  id: InfantryExplode
  effect: changed
- type: key
  id: InfantryHeadPop
  effect: changed
- type: key
  id: InfantryNuked
  effect: changed
- type: key
  id: InfantryVirus
  effect: changed
- type: key
  id: IonBeam
  effect: changed
- type: key
  id: IonBlast
  effect: changed
- type: format
  id: multiplayer-rules
  effect: changed
credit: [MentalHomiega]
---

Animation keys that Yuri's Revenge keeps in `[General]`, such as `InfantryExplode` and `BridgeExplosions`, and the lobby defaults it keeps in `[MultiplayerDialogSettings]`, are now read from those sections. They used to stay unset with Yuri's Revenge rules.
