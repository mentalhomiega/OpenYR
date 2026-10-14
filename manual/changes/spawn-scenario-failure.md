---
title: End a launched game when its scenario cannot be read
category: fix
release: 0.2.0
targets:
- type: command
  id: launch:spawn
  effect: changed
credit:
- MentalHomiega
---

A game started with `-SPAWN` now exits after it shows the error for a scenario it cannot read. It no longer goes on to start the match on an empty map.
