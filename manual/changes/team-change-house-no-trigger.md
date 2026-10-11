---
title: Change a team's house without springing the entered trigger
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_CHANGE_HOUSE
  effect: changed
credit:
- MentalHomiega
---

The Change house team line now hands its members over as a mind control capture does, so a member's "entered by" trigger no longer springs. Members given to a house a person plays leave the team, as they do when mind controlled. Before this the line treated each member as captured by an engineer.
