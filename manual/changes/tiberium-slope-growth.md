---
title: Keep Tiberium off slopes
category: fix
release: 0.2.0
targets:
- type: system
  id: tiberium
  effect: changed
credit: [MentalHomiega]
---

Tiberium no longer grows on a sloped cell, and it no longer spreads from one or onto one. Before, the four simple slopes took new Tiberium through their slope overlays, and a type whose set had none could waste its spread passes. A new spread cell also starts at stage 3, as in Yuri's Revenge, instead of stage 5.
