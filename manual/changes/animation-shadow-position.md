---
title: Draw an upright animation's shadow after the picture at its position
category: fix
release: 0.2.0
targets:
- type: key
  id: Shadow
  scope: animtype
  effect: changed
- type: key
  id: YDrawOffset
  effect: changed
credit: [MentalHomiega]
---

The shadow frames of an upright animation with [`Shadow=yes`](/keys/shadow/#scope-animtype) are now drawn after the picture and at the animation's position, without [`YDrawOffset`](/keys/ydrawoffset/), as in Yuri's Revenge. Before, the shadow was drawn first and moved with `YDrawOffset`. The shadow is also no longer depth-tested, so objects in front of the animation no longer hide it.
