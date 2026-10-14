---
title: Damage a wood low bridge once for each weapon impact
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A weapon impact on a wood low bridge damaged the span twice, so one shell could drop the span two stages at once. Now each impact damages it once, as gamemd does. Checked on KILLER with one ion cannon impact on the wood span at 66,67: the span moves one stage, from overlay 76 to 80.
