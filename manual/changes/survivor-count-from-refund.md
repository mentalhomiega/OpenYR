---
title: Count survivors from the sale refund
category: fix
release: 0.2.0
targets:
- type: key
  id: AlliedSurvivorDivisor
  effect: added
- type: key
  id: SovietSurvivorDivisor
  effect: added
- type: key
  id: ThirdSurvivorDivisor
  effect: added
- type: key
  id: SurvivorRate
  effect: removed
- type: key
  id: SurvivorDivisor
  effect: removed
credit: [MentalHomiega]
---

A destroyed or sold crewed structure now takes its survivor count from its sale refund divided by a divisor for its owner's place in `[Sides]`, limited to between one and five, as Yuri's Revenge does. The divisors are `AlliedSurvivorDivisor` for the first side, `SovietSurvivorDivisor` for the second and `ThirdSurvivorDivisor` for the third; `SurvivorRate` and `SurvivorDivisor` are no longer read. The survivors are the crew type of that place. With the shipped values, a first-side structure that costs 2,000 credits and refunds 1,000 now releases two survivors instead of five, and a house on no side, or on a side after the third, releases no survivors.
