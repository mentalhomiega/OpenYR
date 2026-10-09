---
title: Hold lost computer structures back after an attack
category: fix
release: 0.2.0
targets:
- type: key
  id: AIRestrictReplaceTime
  effect: added
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

We hold a lost computer structure back for `AIRestrictReplaceTime` frames after one of the house's structures takes damage, in skirmish and multiplayer games. Armed structures and power plants are rebuilt at once, and a wall is rebuilt only beside one of the house's structures. A construction yard is held back like other structures. A node that has never held a structure is filled at once. Before, the computer placed a lost structure again as soon as its node was empty, so it refilled its base during an attack. The default rules set the key to 400. A rules file that omits it gets 0, which keeps the old timing. Base plan nodes save a placed flag, so saves from earlier builds are not compatible.
