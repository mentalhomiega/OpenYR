---
title: Show rendered and game rates in the ShowFPS overlay
category: feature
release: 0.2.0
targets:
- type: key
  id: ShowFPS
  effect: changed
credit:
- MentalHomiega
---

`ShowFPS=yes` now draws two counts. `Render` is the pictures per second that reached the display, and `Game` is the game ticks per second. Before, one count gave the game ticks only, so pictures drawn between game frames could not be told apart from the game speed. The game's tick count, which gates the detail effects, is unchanged.
