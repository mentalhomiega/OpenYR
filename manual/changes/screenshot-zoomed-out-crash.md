---
title: Stop the game crashing on a screenshot taken while the map is zoomed out
category: fix
release: 0.2.0
targets:
- type: command
  id: ScreenCapture
  effect: changed
credit:
- MentalHomiega
---

A `ScreenCapture` shot taken while the map is zoomed out crashed the game. The shot now scales the zoomed map to the screen the same way the live display does.
