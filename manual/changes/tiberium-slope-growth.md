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

We stopped Tiberium from growing or spreading onto a slope, as in Yuri's Revenge. Before, the four simple slopes took new Tiberium through their slope overlays, and a type whose set had none could waste its spread passes. A spreading patch also starts at stage 3 instead of stage 5.
