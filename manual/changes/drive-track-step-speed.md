---
title: Drive vehicles at Yuri's Revenge's speed
category: fix
release: 0.2.0
targets:
- type: key
  id: Speed
  effect: changed
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

Vehicles moved by the drive locomotor, such as tanks, harvesters and wheeled vehicles, now travel at the speed they have in Yuri's Revenge. They were about 14% too fast: a Rhino cruised at 28 leptons a frame instead of 24. Infantry, hovercraft and every other locomotor are unchanged. [Movement and terrain](/systems/movement-and-terrain/#why-it-is-slower-than-its-speed-says) gives the distance a vehicle covers per frame.
