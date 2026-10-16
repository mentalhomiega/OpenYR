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

A game started with `-SPAWN` now exits as soon as it shows the error for a scenario it cannot read. It no longer starts the menu music again before it exits.
