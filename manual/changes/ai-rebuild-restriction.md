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

We hold a lost computer structure back for `AIRestrictReplaceTime` frames after one of the house's structures takes damage. The hold-back applies in skirmish and multiplayer games. Walls, base defenses and power plants are still rebuilt at once. Before, the computer placed a lost structure again as soon as its node was empty, so it refilled its base during an attack. The default rules set the key to 400. A rules file that omits it gets 0, which keeps the old timing. The save revision moves to 26, so saves from earlier builds are not compatible.
