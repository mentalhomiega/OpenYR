---
title: Stop the lobby's menu battle while a CnCNet match runs
category: performance
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

While a match started from the built-in CnCNet lobby runs, the lobby hides its window and pauses its menu battle and music. The room keeps its connection and chat. When the match ends, the lobby shows its window again and the menu battle and music carry on from where they stopped.

Before this change, the lobby kept simulating and drawing the menu battle behind a running match. In a local test with a host and a stand-in second player, the lobby process used about 8 percent of one processor core over a 10-second sample of the match. With the change, the same sample used about 0.5 percent. The menu battle's frame count stayed at 2084 from the start of the match to its end.
