---
title: Set off death weapons when exploding objects die
category: feature
release: 0.2.0
targets:
- type: key
  id: Explodes
  scope: aircrafttype
  effect: changed
- type: key
  id: DeathWeapon
  effect: added
- type: key
  id: DeathWeaponDamageModifier
  effect: added
- type: key
  id: CollateralDamageCoefficient
  effect: changed
- type: key
  id: ExpSpread
  effect: changed
credit: [Lucas]
---

A dying `Explodes=yes` object now sets off its `DeathWeapon`, its primary weapon, or the rules' default death weapon, as Yuri's Revenge does, instead of the Tiberian Sun collateral blast. `CollateralDamageCoefficient` and `ExpSpread` no longer have an effect.
