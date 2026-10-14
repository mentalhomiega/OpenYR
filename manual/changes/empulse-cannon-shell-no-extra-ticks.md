---
title: Launch an EMP cannon shell without two extra ticks
category: fix
release: 0.2.0
targets:
- type: key
  id: EMPulseCannon
  effect: changed
- type: system
  id: projectile-flight
  effect: changed
credit:
- MentalHomiega
---

An EMP pulse cannon's launch shell took two extra AI ticks in the frame it was fired, so it moved three times that frame and could detonate there. The extra ticks ran only when the cannon's first weapon had a `PBarrelLength` above `0`. Now the shell takes no tick in its firing frame and takes its first flight turn later in that frame, as other shots do. gamemd's EM pulse branch of `BuildingClass::Mission_Missile` (0x44C980) calls no bullet update after the shell is launched, so the extra ticks did not match it.
