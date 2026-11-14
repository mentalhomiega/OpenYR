---
title: Let a weapon use more or fewer rounds a shot (Ammo)
category: feature
release: 0.2.0
targets:
- type: key
  id: Ammo
  effect: added
  scope: weapontype
credit:
- MentalHomiega
---

A weapon now reads `Ammo=` as Ares does: each shot uses that many of its firer's rounds, an object with fewer rounds does not fire the weapon, and `Ammo=0` uses none and leaves the reload countdown running. Aircraft still use one round a shot. Weapons that leave the key out behave as before.
