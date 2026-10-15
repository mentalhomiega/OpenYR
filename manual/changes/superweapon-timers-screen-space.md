---
title: Draw superweapon timers on the screen at a fixed size, and blink a ready timer
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

The countdown list of superweapons is drawn over the screen rather than over the battlefield, so it keeps its size when the view is zoomed and no longer flickers while the map redraws. A timer for a weapon that is ready blinks once a second.

At zoom levels above one, the list is hidden with the rest of the interface in this build.
