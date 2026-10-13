---
title: A full superweapon charge is ready on the next frame
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_SET_SUPER_CHARGE
  effect: changed
credit:
- MentalHomiega
---

Setting a superweapon to 100 percent now leaves one frame of charge, as the original game does. The charged share is the percentage times a single-precision 0.01, truncated, so a 5,400-frame recharge at 100 percent leaves 1 frame. Before this change the weapon was ready at once.
