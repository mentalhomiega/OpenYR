---
title: Hold a vehicle's turret after it fires
category: fix
release: 0.2.0
targets:
- type: key
  id: GuardAreaTargetingDelay
  effect: changed
credit:
- MentalHomiega
---

A vehicle's turret now stays pointed at its last target for [`GuardAreaTargetingDelay`](/keys/guardareatargetingdelay/) plus 5 frames after the vehicle's last shot, 41 frames in the stock rules, as gamemd's does. Before, it turned back to the hull the moment the target died or went out of reach. The turret also stays put while a vehicle is deployed or in a bunker, and a turret whose weapon has `OmniFire=yes` no longer turns toward its target.
