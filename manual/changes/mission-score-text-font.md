---
title: Print the score screen text in the game's bitmap font
category: fix
release: 0.2.0
targets:
- type: system
  id: multiplayer-score-screen
  effect: changed
credit:
- MentalHomiega
---

The text of the Mission Score screen after a campaign win uses the game's `GAME.FNT` font, as Yuri's Revenge's dialog text does. Before, it used the shape font `FULLFNT3.SHP`, which drew thinner glyphs with wider spacing, so the debrief wrapped a word earlier. The multiplayer score screen prints through the same stage and now uses the same font and yellow text colour.
