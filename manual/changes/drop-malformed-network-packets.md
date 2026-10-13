---
title: Drop malformed network packets
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
credit:
- MentalHomiega
---

A multiplayer game now drops a malformed network packet instead of reading past its end or crashing. A packet is malformed when it is cut short, has a text field with no terminator, names a color or house outside the range the game uses, sets a game speed or difficulty outside its range, or offers a scenario file with an unsafe name or an impossible size. The game logs the first drop of each kind to its debug log. Valid packets are handled as before.
