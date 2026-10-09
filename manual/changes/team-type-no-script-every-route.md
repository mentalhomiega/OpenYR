---
title: Stop a TeamType with no Script from crashing every team route
category: fix
release: 0.2.0
targets:
- type: key
  id: Script
  effect: changed
- type: action
  id: TACTION_CREATE_TEAM
  effect: changed
- type: mission
  id: TMISSION_TEAMCHANGE
  effect: changed
credit: [MentalHomiega]
---

A team made from a TeamType with no `Script=` no longer crashes the game when a create-team action, an AI autocreate or a change-team mission makes it. We give such a TeamType the Script that the reinforcement actions already gave it: one zero-length Guard mission. The team ends at once and is disbanded. Before, the first update of the team crashed. gamemd's create-team path adds no Script, so the Guard here is our addition.
