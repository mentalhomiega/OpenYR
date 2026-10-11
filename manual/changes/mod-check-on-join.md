---
title: Check the players' mods when joining a LAN game
category: feature
release: 0.2.0
targets:
- type: system
  id: mod-check
  effect: added
credit:
- MentalHomiega
---

A player who joins a LAN game now sends the mods in force on his machine, and the host refuses him when they differ from its own. Both sides see which mods differ, unless a list is too long to send in full. Then the message says only that the lists differ, and gives the number of mods in each long list. A player with no mods joins a host with no mods as before, including from a build without the check. Games started from a launch file are not checked.
