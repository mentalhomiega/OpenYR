---
title: Hide low-damage particles and combat flashes at low frame rates
category: fix
release: 0.2.0
targets:
- type: key
  id: DetailMinFrameRateNormal
  effect: changed
- type: key
  id: IsLaser
  effect: changed
credit: [MentalHomiega]
---

Like the original game, the frame-rate test now also hides particles whose type deals no damage, and combat flashes that disable no color channel, while the rate is below [`DetailMinFrameRateNormal`](/keys/detailminframeratenormal/#scope-global-rules). A combat flash is also not made at detail level `0` unless its warhead disables a channel. A laser beam is drawn as plain shaded lines while the rate is low. The rate is counted over 960 milliseconds, as in the original game, so the FPS readout runs about 4 percent lower than before.
