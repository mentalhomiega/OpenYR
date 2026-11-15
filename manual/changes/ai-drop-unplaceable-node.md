---
title: Drop a computer base node whose structure finds no place
category: fix
release: 0.2.0
targets:
- type: key
  id: AIPlacementSearchFailures
  effect: added
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

In a skirmish or multiplayer game, a computer house now removes a base node from its plan once the placement search has found no place for its structure `AIPlacementSearchFailures` times, 3 by default. Before, a house whose base had no room for a planned structure started and abandoned that structure without end and built nothing after it in its plan. An upgrade with nothing to go on is now removed after one failure when it is the node the house would build next, also when an earlier node of the plan waits to be rebuilt. Saves from earlier builds are not compatible.
