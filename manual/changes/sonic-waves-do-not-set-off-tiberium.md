---
title: Sonic waves no longer set off Tiberium
category: fix
release: 0.2.0
targets:
- type: key
  id: ChainReaction
  effect: changed
- type: key
  id: AmbientDamage
  effect: changed
- type: key
  id: IsSonic
  effect: changed
- type: key
  id: Power
  effect: changed
- type: system
  id: tiberium
  effect: changed
credit:
- MentalHomiega
---

A sonic wave no longer sets off the Tiberium in the cells it covers, as gamemd does. The detonation that spread to neighboring cells is gone with it. An explosion still thins the Tiberium of a `ChainReaction=yes` overlay, and a `TiberiumChainReaction=yes` animation still clears its cell.
