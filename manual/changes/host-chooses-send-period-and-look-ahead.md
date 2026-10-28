---
title: Let the host choose the send period, look-ahead and protocol
category: feature
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

The host of a built-in CnCNet room or a LAN game can type `/FrameSendRate`, `/MaxAhead` and `/ProtocolVersion` in the lobby chat. They fix how many frames go into one packet, how far ahead orders are scheduled, and whether the match plays protocol 2 or protocol 0. `auto` hands a value back to the game. The room shows the current values to every player.

Launch files read `FrameSendRate` and `MaxAhead` too. The CnCNet client's own defaults (`FrameSendRate=2` or `7` with no `MaxAhead`) count as no choice, so matches started from the stock client play as before. `HostTiming=yes` makes the game take the values literally, which is how the built-in lobby writes them.
