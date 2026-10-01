---
title: Take objects over with mind control
category: feature
release: 0.2.0
targets:
- type: system
  id: mind-control
  effect: added
- type: key
  id: MindControl
  effect: added
- type: key
  id: InfiniteMindControl
  effect: added
- type: key
  id: ControlledAnimationType
  effect: added
- type: key
  id: LeptonMindControlOffset
  effect: added
- type: key
  id: YuriMindControlSound
  effect: added
- type: key
  id: MindClearedSound
  effect: added
- type: key
  id: MindControlAttackLineFrames
  effect: added
- type: key
  id: OverloadCount
  effect: added
- type: key
  id: OverloadDamage
  effect: added
- type: key
  id: OverloadFrames
  effect: added
- type: key
  id: MasterMindOverloadDeathSound
  effect: added
- type: key
  id: ImmuneToPsionics
  effect: changed
- type: key
  id: MindControlRingOffset
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [Lucas]
---

A primary weapon with a `MindControl=yes` warhead now takes its target over for the firer's house, as in Yuri's Revenge. The firer holds up to the weapon's `Damage` in objects, which go back to their houses when it dies. An `InfiniteMindControl=yes` firer holds any number and takes overload damage.
