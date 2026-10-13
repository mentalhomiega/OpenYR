---
title: End a sonic wave 21 frames after it is fired
category: fix
release: 0.2.0
targets:
- type: key
  id: IsSonic
  effect: changed
credit: [MentalHomiega]
---

A sonic wave now starts its trailing edge on its second frame, even while the firer still targets the wave's object, and is removed on its 21st frame. Before, a tethered wave kept growing until about 80 frames after it was fired and then faded for 20 more. A cell near the firer took a hit on about 100 frames, and a target four cells away was hit on 82 of them. Now that target is hit on five frames.
