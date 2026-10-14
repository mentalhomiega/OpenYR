---
title: Let an engineer walk to a bridge repair hut that another house owns
category: fix
release: 0.2.0
targets:
- type: key
  id: BridgeRepairHut
  effect: changed
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

An engineer ordered onto a bridge repair hut now walks to it and repairs the span through it, even when a Civilian or another house owns the hut. Before, the order was dropped as soon as it was given. The engineer checked the zone of its own position where it should have checked where it was going, so a hut that stood in a different zone was out of reach. Checked on SOV04DMD with a Civilian hut at 99,44 on ground an engineer can enter. A hut on ground that refuses infantry still refuses the order.
