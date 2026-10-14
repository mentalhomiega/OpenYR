---
title: End a watched match at the first defeat, as gamemd does
category: fix
release: 0.2.0
targets:
- type: system
  id: observers
  effect: changed
- type: system
  id: leaving-a-match
  effect: changed
- type: format
  id: spawn-ini
  effect: changed
- type: action
  id: TACTION_HOUSE_DESTROY_ALL
  effect: changed
credit:
- MentalHomiega
---

In a match where every person watches, the first defeat ends the match, as it does in gamemd. Before, the match ran until the last enemy of a surviving house fell. Other matches keep their rule: after a defeat, the match ends when the houses still alive are one side or when no person is still alive.

A watcher whose match ends sees the loss caption without the defeat voice line. Before, it saw no caption.

In a network or skirmish game, a win and a loss set on the same frame resolve to the win, as gamemd's end-of-game handling does. A campaign still resolves to the loss.
