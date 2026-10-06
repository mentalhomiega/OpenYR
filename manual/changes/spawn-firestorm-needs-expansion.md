---
title: Load saves from spawned games without the expansion installed
category: fix
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

`Firestorm=yes` in the spawn file now asks for the expansion only when it is installed. A game started through the spawn file on a deployment without the expansion, such as Yuri's Revenge, used to save as needing it, and then refused to load that save.
