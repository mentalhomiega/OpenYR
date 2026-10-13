---
title: Log the frame check value from RA2MD.INI
category: feature
release: 0.2.0
targets:
- type: key
  id: SyncLog
  effect: added
credit:
- MentalHomiega
---

`SyncLog=yes` in the `[Options]` section of `RA2MD.INI` writes the frame check value to the debug log on every game frame, as the `-SYNCLOG` switch does. A launch file can set the same key in `[Settings]`. The setting does not change the match or its session identity, so builds that lack the key play the same game.
