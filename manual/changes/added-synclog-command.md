---
title: Accept -SYNCLOG to log the frame check value
category: feature
release: 0.2.0
targets:
- type: command
  id: launch:sync-log
  effect: added
credit:
- MentalHomiega
---

`-SYNCLOG` writes the frame check value to the debug log on every game frame. Comparing two machines' logs shows the first frame where their check values differ. The switch does not change the match.
