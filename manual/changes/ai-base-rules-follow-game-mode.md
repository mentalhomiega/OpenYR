---
title: Decide the computer's power plants and defense nodes by game mode
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
- type: key
  id: UseMPAIBaseNodes
  effect: changed
credit:
- MentalHomiega
---

A computer house in a campaign game now inserts a power plant ahead of a structure it cannot power, as it does in Yuri's Revenge. Before, only skirmish and multiplayer houses did.

On a map that sets `UseMPAIBaseNodes=yes`, a destroyed base defense's node is now turned back into a placeholder, as in any skirmish. Before, the key kept the node and the house rebuilt the same defense in place.
