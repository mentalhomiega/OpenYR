---
title: Add the BodyFrameFromIdle key and stop shadow frames showing on a structure's body
category: fix
release: 0.2.0
targets:
- type: key
  id: BodyFrameFromIdle
  effect: added
- type: system
  id: building-animations
  effect: changed
credit:
- MentalHomiega
---

A structure whose body animation reaches the shadow half of its artwork now draws its idle body frame, so an Allied Construction Yard no longer loses its hangar for about half a second when it places a structure. `BodyFrameFromIdle=` in `[AudioVisual]` sets this. The default, `yes`, draws the idle frame. `no` draws the shadow frame, which is what the clamp in Yuri's Revenge's draw code gives.
