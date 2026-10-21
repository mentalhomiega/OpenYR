---
title: Show only the nullify animation for a turned-away shot
category: fix
release: 0.2.0
targets:
- type: key
  id: WeaponNullifyAnim
  effect: changed
credit: [MentalHomiega]
---

A shot that an Iron Curtain turns away now shows only WeaponNullifyAnim, as in Yuri's Revenge. Before, it could still draw its own light, and with WeaponNullifyAnim empty it drew its own blast animation.
