---
title: Start an enter order while in contact with a war factory
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

We now start a queued enter order for a vehicle that is in radio contact with a `WeaponsFactory=yes` structure, as we already start a queued move order. Other orders still wait for that contact to end. Before, a naval yard's dock kept the vehicle in contact, so the enter order stayed queued until the vehicle reached the dock and then went idle there.
