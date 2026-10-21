---
title: Start Cooperative players at the positions a map keeps for them
category: fix
release: 0.2.0
targets:
- type: system
  id: starting-forces
  effect: changed
credit:
- MentalHomiega
---

In a Cooperative match, people now start at one of the first start positions of the map, as many as the map's `[Header]` key `NumCoopHumanStartSpots=` says, and computer players start at the positions after them. Before, every house drew a position among all of them, so a person could start where the computer's base is placed. See [the start position](/systems/starting-forces/#the-start-position).
