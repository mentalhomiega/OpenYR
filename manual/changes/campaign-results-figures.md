---
title: Show the campaign results figures as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: ScoreArtFillsStage
  effect: added
credit:
- MentalHomiega
---

The campaign results screen after a won mission fills its 800 by 600 stage with the side's art. The rows name the player and the first enemy house, and the house with the higher score is listed first. Kills and losses are counts. Built is the house's losses plus the objects it still owns. Score is the house's point total. Built and score print as plain numbers, not percentages.

The first and second places repeat the two rows, so the screen no longer keeps a hall-of-fame file. `ScoreArtFillsStage=no` in `[General]` of the rules keeps the art at its 632 by 568 size in the top left corner. The original's placement of the art outside that 632 by 568 size is not confirmed.
