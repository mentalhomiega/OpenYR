---
title: Let a tech secret lab's owner build one item without prerequisites
category: feature
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
- type: key
  id: SecretLab
  effect: added
- type: key
  id: SecretInfantry
  effect: added
- type: key
  id: SecretUnit
  effect: added
- type: key
  id: SecretBuilding
  effect: added
- type: key
  id: SecretUnits
  effect: added
- type: key
  id: SecretBuildings
  effect: added
credit:
- MentalHomiega
---

A structure with `SecretLab=yes`, such as the tech secret lab, now lets its owner build one infantry, vehicle or structure type without meeting its prerequisites, as Yuri's Revenge does. When a skirmish or network game starts, each lab draws its item from the `SecretInfantry`, `SecretUnits` and `SecretBuildings` lists in `[General]`. `SecretInfantry=`, `SecretUnit=` or `SecretBuilding=` on the lab's type names the item instead, in campaign missions too. Capturing a lab moves its item to the new owner, and a lab taken off the map no longer offers it. Saves made by earlier builds no longer load, because labs, houses, building types and the rules now save these settings.
