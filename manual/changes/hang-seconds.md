---
title: Set the hang watchdog timeout with HangSeconds
category: feature
release: 0.2.0
targets:
- type: key
  id: HangSeconds
  effect: added
credit:
- MentalHomiega
---

`HangSeconds=` in `[Debug]` of `RA2MD.INI` sets how long the main thread may go without handling messages before the game reports a hang. The default is 5 seconds, `0` turns the check off, and a value above 3600 is treated as 3600.
