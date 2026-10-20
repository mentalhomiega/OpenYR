---
title: Stop crashing when an AttachEffect animation is deleted before its object
category: fix
release: 0.2.0
targets:
- type: key
  id: AttachEffect.Animation
  effect: changed
credit:
- MentalHomiega
---

An object whose [`AttachEffect.Animation`](/keys/attacheffect.animation/) was deleted while the object stayed in play, for example when it left the map, kept a pointer to the deleted animation and could crash the game on a later frame. The object now drops the animation when it is deleted, and starts a new one when the effect still runs.
