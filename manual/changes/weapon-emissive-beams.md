---
title: A weapon's Emissive key sets its beam's glow
category: fix
release: 0.2.0
targets:
- type: key
  id: Emissive
  scope: weapontype
  effect: changed
credit: [MentalHomiega]
---

[`Emissive`](/keys/emissive/#scope-weapontype) on a weapon with `IsLaser=yes` or `IsElectricBolt=yes` now sets the glow strength of the beam or bolt the weapon's own shots draw. Before, only the secondary bolts of a fragmenting projectile read it, and every ordinary shot used the default.
