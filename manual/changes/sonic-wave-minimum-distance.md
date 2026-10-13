---
title: Skip sonic and magnetron waves at point-blank range
category: fix
release: 0.2.0
targets:
- type: key
  id: IsSonic
  effect: changed
- type: key
  id: IsMagBeam
  effect: changed
credit: [MentalHomiega]
---

A sonic wave fired at a target less than 240 leptons away is no longer drawn and no longer damages anything, as in Yuri's Revenge. A magnetron beam at such a target is no longer drawn. The weapon still fires and spends its ammunition and reload. Before, a sonic wave at point-blank range rolled out and damaged everything under it.
