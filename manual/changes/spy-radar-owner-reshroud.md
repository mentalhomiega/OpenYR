---
title: Reshroud only the local player's map after a spy enters a radar
category: fix
release: 0.2.0
targets:
- type: system
  id: capture
  effect: changed
credit:
- MentalHomiega
---

A spy that enters a radar now shrouds the map again only when the structure's owner is the local player. We used to shroud the owner's map whatever its house, which also cleared a computer player's mapped cells. A working spy satellite still keeps the map.
