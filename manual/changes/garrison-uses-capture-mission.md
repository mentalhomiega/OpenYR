---
title: Match gamemd for the mission a garrison order runs on
category: fix
release: 0.2.0
targets:
- type: key
  id: Occupier
  scope: infantrytype
  effect: changed
- type: key
  id: Assaulter
  scope: infantrytype
  effect: changed
credit:
- MentalHomiega
---

A soldier sent into a structure to garrison it now runs the Capture mission, as in Yuri's Revenge, whether the player clicks the structure, a computer's soldier hunts or attacks it, or a team script sends the soldier. Before, it ran the Enter mission.

A computer's `Occupier=yes` soldier on the Capture mission with no target stays on that mission, and a computer's `Assaulter=yes` soldier in that case goes back to hunting. A soldier with `Occupier=yes`, `Assaulter=yes` or `Infiltrate=yes` that has a structure as its target now sets out for it on the Capture mission.
