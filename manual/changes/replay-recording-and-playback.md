---
title: Record a skirmish and play it back to the same result
category: feature
release: 0.2.0
targets:
- type: key
  id: RecordReplays
  effect: added
- type: command
  id: launch:replay
  effect: added
credit: [MentalHomiega]
---

`RecordReplays=yes` under `[Options]` in `RA2MD.INI` records each skirmish that a launch file starts to the `Replays` folder, and `-REPLAY=<file>` plays a recording back. Playback runs the recorded orders on their frames, ignores the player's orders, and compares hashes of the game state with the ones the recording holds. It is off by default.
