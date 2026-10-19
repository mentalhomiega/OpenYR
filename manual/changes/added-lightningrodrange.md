---
title: Add the lightning rod range key
category: feature
release: 0.2.0
targets:
- type: key
  id: LightningRodRange
  effect: added
credit:
- MentalHomiega
---

`LightningRodRange=` in `[General]` sets how many cells away a lightning storm cloud is pulled to a structure with `LightningRod=yes`. The default, `4`, applies to a storm whose weapon does not set `Lightning.IgnoreLightningRod=yes`; the stock lightning storm has no structure with `LightningRod=yes`, so it is unchanged.
