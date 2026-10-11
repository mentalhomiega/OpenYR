---
title: Read ThirdPowerPlant for the third side's power plant
category: feature
release: 0.2.0
targets:
- type: key
  id: ThirdPowerPlant
  effect: added
credit:
- MentalHomiega
---

A computer house playing for the third side, Yuri's in the stock rules, now takes its power plant from `ThirdPowerPlant` when its plan runs short of power. Before, the key was ignored and the house inserted the first `BuildPower` entry its country may own, which is not always the side's own plant. The save revision moves to 40, so saves from earlier builds are not compatible.
