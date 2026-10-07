---
title: Keep a ready superweapon's animation away during a power shortfall
category: fix
release: 0.2.0
targets: []
credit: [MentalHomiega]
---

A structure whose `SuperAnimThree` has `SuperAnimThreePoweredEffect=yes`, such as the Genetic Mutator, now shows its `SuperLowPower` animation alone while its house is short of power. Before, the ready animation was started again over it on every update.
