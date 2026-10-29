---
title: Make a destroyed Crashable vehicle fall and crash
category: fix
release: 0.2.0
targets:
- type: key
  id: Crashable
  effect: changed
credit:
- MentalHomiega
---

A vehicle with `Crashable=yes` and a jumpjet locomotor, such as the Kirov, the Siege Chopper and the Floating Disc, no longer vanishes when it is destroyed in the air. It explodes where it dies, then falls spinning and tilting at its `JumpjetCrash` speed and explodes again where it lands. A `BalloonHover` vehicle sets off its death weapon on landing instead. The vehicle crushes nothing and is removed on landing.
