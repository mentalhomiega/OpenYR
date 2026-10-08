---
title: Time the lightning bolts and clouds as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

We drop a lightning bolt once its cloud's animation is past halfway, not on the halfway frame, and we keep each cloud until its last frame. Before, a bolt dropped on the halfway frame, and a cloud left the storm's list about halfway through its animation, so the spacing check ignored it for the rest of the animation.
