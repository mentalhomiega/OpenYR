---
title: Record and watch skirmish replays from the menus
category: feature
release: 0.2.0
targets:
- type: key
  id: RecordReplays
  effect: changed
- type: command
  id: ReplayPause
  effect: added
- type: command
  id: ReplayFast
  effect: added
credit: [MentalHomiega]
---

A skirmish started from the skirmish lobby is recorded when [`RecordReplays=`](/keys/recordreplays/) is on. The main menu of the modern interface has a Replays entry that lists the recordings with their map, players, date, length and result, plays one, and deletes one. A recording this game cannot play says why. The Game tab of the options has a Record replays switch for the same setting.

During playback a label at the top of the view shows the frame, and Pause and the numeric keypad's plus key pause the replay and play it without the frame limit. Clicks on the map and the order keys give no orders.

Replays made by earlier builds use an older file format and do not play.
