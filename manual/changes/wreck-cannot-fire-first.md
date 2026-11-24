---
title: Refuse fire from a vehicle wreck before other checks
category: fix
release: 0.2.0
targets:
- type: system
  id: destruction-and-debris
  effect: changed
credit:
- MentalHomiega
---

A vehicle wreck now always answers that it cannot fire, as the original game does. Before, a wreck of a Visceroid type could still fire, and a wreck with missiles leaving it or a linked unit gave a different refusal.

A soldier playing a death sequence now also refuses to fire. Before, it could still fire while the sequence played.
