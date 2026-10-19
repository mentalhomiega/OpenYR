---
title: Give warheads Iron Curtain, weapon jam, hit limit and kill-feedback keys
category: feature
release: 0.2.0
targets:
- type: key
  id: IronCurtain.Duration
  effect: added
- type: key
  id: IronCurtain.Cap
  effect: added
- type: key
  id: DisableWeapons.Duration
  effect: added
- type: key
  id: CellSpread.MaxAffect
  effect: added
- type: key
  id: UnitLost.Suppress
  effect: added
- type: key
  id: DieSound.Override
  effect: added
- type: key
  id: VoiceDie.Override
  effect: added
- type: key
  id: AffectsOwner
  effect: added
- type: key
  id: EMP.Sparkles
  effect: added
credit:
- MentalHomiega
---

A warhead can put the objects it hits under the Iron Curtain with `IronCurtain.Duration=` and `IronCurtain.Cap=`, keep them from firing with `DisableWeapons.Duration=`, and show its own animation on an EMP stun with `EMP.Sparkles=`. `CellSpread.MaxAffect=` limits how often one object is hit by one blast. `UnitLost.Suppress=`, `DieSound.Override=` and `VoiceDie.Override=` silence what the objects it destroys announce, and `AffectsOwner=` decides whether it hits the firer's own objects. The keys follow the Ares documentation. A warhead that sets none of them behaves as before.
