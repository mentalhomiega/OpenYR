---
title: Carry Flak Tracks inside a naval reinforcement transport
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_REINFORCEMENTS
  effect: changed
- type: key
  id: Full
  effect: changed
credit:
- MentalHomiega
---

A reinforcement team with a naval member takes only naval vehicles as transports, and a team that is not `Full` sends no transports. A Flak Track in such a team now rides inside the naval transport instead of arriving as a second transport.
