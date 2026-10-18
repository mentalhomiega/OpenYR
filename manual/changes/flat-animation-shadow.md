---
title: Draw flat animations without a shadow
category: fix
release: 0.2.0
targets:
- type: key
  id: Flat
  scope: animtype
  effect: changed
- type: key
  id: Shadow
  scope: animtype
  effect: changed
credit: [MentalHomiega]
---

A flat animation (`Flat=yes`) no longer draws its shadow frames, even when it sets [`Shadow=yes`](/keys/shadow/#scope-animtype). Before, the shadow darkened the ground under it. A flat animation's depth also moves by one pixel, as in Yuri's Revenge.
