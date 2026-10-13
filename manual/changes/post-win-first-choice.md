---
title: Take the first choice after a campaign win without a map screen
category: fix
release: 0.2.0
targets:
- type: system
  id: campaign-progression
  effect: changed
- type: format
  id: mapsel-ini
  effect: changed
credit:
- MentalHomiega
---

A campaign mission won without `SkipMapSelect=yes` no longer opens the map selection screen. The game takes the first choice of the mission's stage, and it plays the `LOADING` track when the stage has a `MapVQ=`, as the original game's post-win code does. Before, the screen opened when its art was present, and the first choice was taken only when the art was missing. A stage with no `MapVQ=` still loads the mission just won again.
