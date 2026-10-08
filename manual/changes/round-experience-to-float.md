---
title: Round experience to a float and cap a zero divisor
category: fix
release: 0.2.0
targets:
- type: system
  id: veterancy
  effect: changed
credit: [MentalHomiega]
---

We now keep each object's experience as a float, as Yuri's Revenge does, so a total that rounds up to a rank threshold promotes the object. When `VeteranRatio` or the killer's cost is `0`, every kill the object is credited with sets its experience to `VeteranCap`. Earlier builds left the total unusable when the victim was worth nothing.
