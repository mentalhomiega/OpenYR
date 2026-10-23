---
title: Keep a held superweapon on hold when debug charge fills it
category: fix
release: 0.2.0
targets:
- type: command
  id: DebugChargeSuperweapons
  effect: changed
credit:
- MentalHomiega
---

`DebugChargeSuperweapons` fills every super weapon the player's house holds. A weapon that is on hold for low power or a chrono erase now stays on hold: it reads ready, but it cannot fire until the hold lifts. Before, the command restarted the countdown of a held weapon, so that weapon could fire while held. [Charging](/systems/superweapons/#charging) describes what the hold does to the countdown.
