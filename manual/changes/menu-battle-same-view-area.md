---
title: Show the menu battle at one height of map
category: feature
release: 0.2.0
targets:
- type: system
  id: map-zoom
  effect: changed
- type: key
  id: MenuBattleViewHeight
  effect: added
credit: [MentalHomiega]
---

The battle behind the main menu now shows 1080 map pixels of height on a screen of any size, so a 4K screen shows the same area as a 1080p one, drawn larger. `MenuBattleViewHeight` under `[General]` in `rules.ini` sets the height. The mouse wheel does not zoom the battle.
