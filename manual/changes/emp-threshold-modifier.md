---
title: Scale and limit EM pulse stuns per type
category: feature
release: 0.2.0
targets:
- type: key
  id: EMP.Modifier
  effect: added
- type: key
  id: EMP.Threshold
  effect: added
credit:
- MentalHomiega
---

In rulesmd.ini, an object type can lengthen or shorten the stun an EM pulse gives it with `EMP.Modifier`, and `EMP.Threshold` destroys it when the stun is longer than the value, or only while it is in the air with `inair`. The keys follow the Ares documentation, without its stun-duration counter.
