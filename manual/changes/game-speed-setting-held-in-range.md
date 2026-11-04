---
title: Hold the GameSpeed setting to the speeds the game can run
category: fix
release: 0.2.0
targets:
- type: key
  id: GameSpeed
  effect: changed
credit:
- MentalHomiega
---

`GameSpeed` in `[Options]` of `RA2MD.INI` is now held to `0` through `7` when the game reads it: a lower value becomes `0` and a higher one `7`, and the game writes the adjusted value back to `RA2MD.INI` the next time it saves its settings. Before, `-1` crashed the game early in a mission, and other values below `0` or above `7` gave animations and EVA warning pauses arbitrary lengths. Values from `0` to `7` behave as before, so `7` still runs a campaign mission or skirmish with no frame limit.
