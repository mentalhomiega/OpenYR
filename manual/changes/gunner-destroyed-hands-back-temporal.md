---
title: Hand a gunner's temporal back when its vehicle is destroyed
category: fix
release: 0.2.0
targets:
- type: system
  id: gunner-vehicles
  effect: changed
credit:
- MentalHomiega
---

When a gunner vehicle is destroyed, each passenger it puts out goes through the same hand-back as an unload. The last one out takes the vehicle's temporal and rearm state, and it lets go of its target. Before this change, the temporal stayed with the destroyed vehicle and no passenger received it.
