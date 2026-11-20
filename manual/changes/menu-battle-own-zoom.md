---
title: Show the menu battle at its own zoom
category: fix
release: 0.2.0
targets:
- type: system
  id: ui-files
  effect: changed
credit:
- MentalHomiega
---

The battle behind the menus now always starts at the zoom a new game starts at. Before, it was drawn at the map zoom you last used in a match, so after playing zoomed in the menus stood over a few huge pixels. Your own zoom comes back when the next match starts, and the battle never changes your Game Speed option.
