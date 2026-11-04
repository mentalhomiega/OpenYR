---
title: Hold the GameSpeed setting to the speeds the game offers
category: fix
release: 0.2.0
targets:
- type: key
  id: GameSpeed
  effect: changed
credit:
- MentalHomiega
---

`GameSpeed` in `[Options]` of `RA2MD.INI` is now held to `0` through `6` when the game reads it: a lower value becomes `0` and a higher one `6`. Before, `-1` crashed the game early in a mission, other values below `0` or above `7` gave animations and EVA warning pauses arbitrary lengths, and `7` ran at the fastest speed and made recordings that could not be played back. Values from `0` to `6` behave as before.
