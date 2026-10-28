---
title: Do not crash when a structure with a projectile-less weapon is hit
category: fix
release: 0.2.0
targets:
- type: key
  id: Projectile
  effect: changed
credit:
- MentalHomiega
---

A structure whose [`Primary=`](/keys/primary/) weapon has no [`Projectile=`](/keys/projectile/) no longer crashes the game when an enemy hits it. Red Alert 2 maps on Yuri's Revenge rules show this: the garrisonable civilian building `CAWA2C` names `AlliedOccupyW`, a weapon `rulesmd.ini` does not define. Such a structure does not return fire at its attacker.
