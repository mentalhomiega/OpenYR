---
title: Win and Lose name the country a map's house plays
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_WIN
  effect: changed
- type: action
  id: TACTION_LOSE
  effect: changed
credit:
- MentalHomiega
---

The Win and Lose actions now find the player when the map's player house plays a country of the map's own, such as `Player House` playing the country `Player`. The action names that country, but the house is a type of its own that stands on it, so the player was never matched and a trigger that won the mission lost it instead. Allied mission 5 and Soviet missions 2 and 7 are made this way.
