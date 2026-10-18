---
title: Skip detail-gated animations at low frame rates and add animation end sounds
category: fix
release: 0.2.0
targets:
- type: key
  id: DetailMinFrameRateNormal
  effect: added
- type: key
  id: DetailBufferZoneWidth
  effect: added
- type: key
  id: DetailLevel
  scope: animtype
  effect: changed
- type: key
  id: StopSound
  scope: animtype
  effect: added
credit:
- MentalHomiega
---

An animation with a [`DetailLevel`](/keys/detaillevel/#scope-animtype) above `1` is now hidden while the frame rate is below [`DetailMinFrameRateNormal`](/keys/detailminframeratenormal/#scope-global-rules), as in the original game. Animation types also gain [`StopSound`](/keys/stopsound/#scope-animtype), which plays when the animation is deleted. A harvester no longer draws its gather sparkles while a temporal warp holds it or it is warping. Existing rules files need no change: the new keys default to the original values.
