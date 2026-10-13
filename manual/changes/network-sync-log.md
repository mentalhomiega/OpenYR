---
title: Log a per-frame check value for network sync tests
category: feature
release: 0.2.0
targets:
- type: command
  id: launch:sync-log
  effect: added
credit:
- MentalHomiega
---

The new `-SYNCLOG` switch writes the game's check value for every frame to the debug log. Two copies in one network game write matching lines while they agree, so comparing their logs shows the first frame at which they differ.
