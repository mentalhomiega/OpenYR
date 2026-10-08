---
title: Show the alternate cameo after a spy enters a barracks or war factory
category: feature
release: 0.2.0
targets:
- type: key
  id: AltCameo
  effect: added
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

Once the local player's house has spied on a barracks or war factory, we draw a trainable type's [`AltCameo`](/keys/altcameo/) in the sidebar. We did not read the key before, so the sidebar kept the normal cameo of every type.
