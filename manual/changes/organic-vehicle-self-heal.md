---
title: Heal organic vehicles at the infantry rate
category: fix
release: 0.2.0
targets:
- type: key
  id: Organic
  effect: changed
- type: key
  id: InfantryGainSelfHeal
  effect: changed
- type: key
  id: UnitsGainSelfHeal
  effect: changed
- type: system
  id: repair
  effect: changed
credit:
- MentalHomiega
---

An `Organic=yes` vehicle now heals under the infantry settings: [`InfantryGainSelfHeal`](/keys/infantrygainselfheal/), [`SelfHealInfantryFrames`](/keys/selfhealinfantryframes/) and [`SelfHealInfantryAmount`](/keys/selfhealinfantryamount/). Before this change it used the vehicle settings, so a house with a hospital and no machine shop never healed its organic vehicles. Vehicles without `Organic=yes` are unchanged.
