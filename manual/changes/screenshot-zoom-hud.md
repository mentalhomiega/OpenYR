---
title: Keep the sidebar and superweapon timers in screenshots taken while the map is zoomed
category: fix
release: 0.2.0
targets:
- type: command
  id: ScreenCapture
  effect: changed
credit:
- MentalHomiega
---

A `ScreenCapture` shot taken with the map zoomed in showed a black frame, with no sidebar, timers or command bar. The shot now places the map only where the frame is keyed out, as the live display does, so the interface stays in the picture.
