---
title: Show commas in map names in network games
category: fix
release: 0.2.0
targets:
- type: key
  id: Description
  scope: map-packets
  effect: changed
- type: key
  id: Description
  scope: multiplayer-maps
  effect: changed
credit:
- MentalHomiega
---

A map name with a comma showed a semicolon in the network lobby of a LAN game, because the host replaced each comma before sending the name. The host now sends the comma as `%2C`, and the joining player's list shows a comma. A build from before this change shows `%2C` where the comma is.
