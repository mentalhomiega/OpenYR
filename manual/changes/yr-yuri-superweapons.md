---
title: Fire the psychic reveal, genetic mutator, force shield and psychic dominator
category: feature
release: 0.2.0
targets:
- type: key
  id: Type
  scope: superweapontype
  effect: changed
- type: key
  id: InfDeath
  effect: changed
- type: key
  id: InfantryMutate
  effect: changed
- type: key
  id: InfantryBrute
  effect: changed
- type: key
  id: MakeInfantry
  effect: added
- type: key
  id: AnimToInfantry
  effect: added
- type: key
  id: MutateExplosion
  effect: added
- type: key
  id: MutateWarhead
  effect: added
- type: key
  id: MutateExplosionWarhead
  effect: added
- type: key
  id: PsychicRevealRadius
  effect: added
- type: key
  id: PsychicRevealActivateSound
  effect: added
- type: key
  id: GeneticMutatorActivateSound
  effect: added
- type: key
  id: PsychicDominatorActivateSound
  effect: added
- type: key
  id: ForceShieldRadius
  effect: added
- type: key
  id: ForceShieldDuration
  effect: added
- type: key
  id: ForceShieldBlackoutDuration
  effect: added
- type: key
  id: ForceShieldPlayFadeSoundTime
  effect: added
- type: key
  id: ForceShieldInvokeAnim
  effect: added
- type: key
  id: DominatorFirstAnim
  effect: added
- type: key
  id: DominatorSecondAnim
  effect: added
- type: key
  id: DominatorFireAtPercentage
  effect: added
- type: key
  id: DominatorDamage
  effect: added
- type: key
  id: DominatorWarhead
  effect: added
- type: key
  id: DominatorCaptureRange
  effect: added
- type: key
  id: PermaControlledAnimationType
  effect: added
- type: key
  id: ImmuneToPsionics
  effect: added
- type: key
  id: BalloonHover
  effect: added
- type: key
  id: MindControlRingOffset
  effect: added
- type: key
  id: StartSound
  scope: superweapontype
  effect: added
- type: key
  id: SpecialSound
  effect: added
- type: system
  id: superweapons
  effect: changed
credit: [Lucas]
---

The psychic reveal, genetic mutator, force shield and psychic dominator superweapons now work as in Yuri's Revenge, and computer houses fire all but the force shield. Infantry killed by an `InfDeath=9` warhead now mutate into new infantry of the attacker's house.
