---
title: Light flamer, scorch and structure fires only when enabled
category: fix
release: 0.2.0
breaking: true
migration:
- Add `AnimationFires=yes` under `[AudioVisual]` in the rules INI of a mod whose Flamer or Scorch animations, or whose damaged and destroyed structures, should keep lighting `SmallFire` and `LargeFire` fires. Without the key, none of them light a fire.
targets:
- type: key
  id: AnimationFires
  effect: added
- type: key
  id: Flamer
  effect: changed
- type: key
  id: Scorch
  effect: changed
- type: key
  id: SmallFire
  effect: changed
- type: key
  id: LargeFire
  effect: changed
- type: key
  id: Sparky
  effect: changed
credit: [MentalHomiega]
---

A [`Flamer`](/keys/flamer/) or [`Scorch`](/keys/scorch/) animation no longer lights a fire unless `AnimationFires=yes` is set in `[AudioVisual]`. A destroyed structure, and a structure damaged by a warhead without [`Sparky=yes`](/keys/sparky/), no longer light `SmallFire` or `LargeFire` fires either. Yuri's Revenge starts no fire from any of them, so `no` is the default. Before, all of them lit their fires whenever `SmallFire` or `LargeFire` was set, and the stock rules set neither. With the key off, the structure rolls for those fires are no longer made, so the random sequence of a game with destroyed or damaged structures differs from earlier builds.
