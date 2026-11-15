---
title: Read up to 64 positions of a numbered weapon list
category: feature
release: 0.2.0
targets:
- type: key
  id: WeaponCount
  effect: changed
credit:
- MentalHomiega
---

A type with `TurretCount=` reads up to 64 weapons, `Weapon1=` to `Weapon64=` with their elite weapons and firing offsets, where it read 18 before. A gunner vehicle can then reach the weapon that the `IFVMode=` of any of 64 passengers names. A type that lists 18 weapons or fewer behaves as before.
