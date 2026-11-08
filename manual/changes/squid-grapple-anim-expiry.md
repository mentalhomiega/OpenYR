---
title: Stop a giant squid's grapple from crashing when its animation ends
category: fix
release: 0.2.0
targets:
- type: system
  id: parasites
  effect: changed
credit:
- MentalHomiega
---

A giant squid could crash the game while it held a victim, once the grapple animation had ended. The squid kept a pointer to that animation and called into it on the next frame. The pointer is now cleared when the animation is removed, and the squid keeps holding its victim without the animation.
