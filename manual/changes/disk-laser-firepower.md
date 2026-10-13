---
title: Apply firepower and veterancy to disk laser damage
category: fix
release: 0.2.0
targets:
- type: key
  id: DiskLaser
  effect: changed
credit: [MentalHomiega]
---

A `DiskLaser` weapon now strikes for the damage a normal shot of that weapon would deal from its firer, as in Yuri's Revenge. The house and object firepower biases, veterancy, bunker, garrison and transport multipliers all apply. Before, the strike used the weapon's `Damage` scaled only by the object's firepower bias and attached effects, so a veteran or garrisoned firer's disk laser hit for less than its other weapons.
