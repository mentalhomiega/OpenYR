---
title: Draw a vehicle's deploying animation through Palette=
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployingAnim
  scope: unittype
  effect: changed
- type: key
  id: Palette
  effect: changed
credit: [MentalHomiega]
---

A vehicle with `IsSimpleDeployer=yes` and `Palette=` in its art now draws its `DeployingAnim` through that palette, in its house's colors, as it deploys and packs up. Before, the animation used its house's colors only. A vehicle without `Palette=` is unchanged.
