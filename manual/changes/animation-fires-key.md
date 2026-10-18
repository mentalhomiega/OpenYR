---
title: Light fires from flamer and scorch animations only when enabled
category: fix
release: 0.2.0
breaking: true
migration:
- Add `AnimationFires=yes` under `[AudioVisual]` in the rules INI of a mod whose Flamer or Scorch animations should keep lighting `SmallFire` and `LargeFire` fires. Without the key, those animations light no fire.
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
credit: [MentalHomiega]
---

A [`Flamer`](/keys/flamer/) or [`Scorch`](/keys/scorch/) animation no longer lights a fire unless `AnimationFires=yes` is set in `[AudioVisual]`. Yuri's Revenge starts no fire from them, so `no` is the default. Before, both lit their fires whenever `SmallFire` or `LargeFire` was set, and the stock rules set neither.
