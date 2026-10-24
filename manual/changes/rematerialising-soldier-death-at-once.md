---
title: Play a soldier's death sequence at once when it dies during a teleport hold
category: fix
release: 0.2.0
targets:
- type: key
  id: Teleporter
  effect: changed
credit:
- MentalHomiega
---

A soldier killed while it holds at its landing after a teleport jump, such as a Chrono Legionnaire or a Chrono Commando, starts its death sequence at once. Before, the sequence did not advance until the hold ended, so the body was removed about ten frames later.
