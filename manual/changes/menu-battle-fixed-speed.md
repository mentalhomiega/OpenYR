---
title: Run the menu battle at a fixed game speed
category: fix
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

Before, the battle behind the menus followed the Game Speed option in `[Options]` of `RA2MD.INI`: it ran one frame a second at the fastest setting and 10 frames a second at the slowest. The battle now always runs at game speed 4, which is 20 frames a second, whatever the option is set to. The option still sets the pace of your own games, except that a skirmish started from the menus takes its speed from the lobby's own Game Speed slider.
