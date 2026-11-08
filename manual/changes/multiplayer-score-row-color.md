---
title: Color each player's score row in the player's house color
category: fix
release: 0.2.0
targets:
- type: system
  id: multiplayer-score-screen
  effect: changed
credit:
- MentalHomiega
---

The name and figures on each player's row of the multiplayer and skirmish score screen are drawn in that player's house color, as in Yuri's Revenge. Before, they were yellow. The title, headings, time and prompt stay yellow. `PlayerColors=` in the options chooses the color each house shows, so the color-blind-safe and relationship views change the rows as well.
