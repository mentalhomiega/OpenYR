---
title: Computer players band together only when AlliesAllowed is on
category: fix
release: 0.2.0
targets:
- type: key
  id: Paranoid
  effect: changed
- type: key
  id: AlliesAllowed
  effect: added
credit: [MentalHomiega]
---

The computer houses now band together against the human players only when `AlliesAllowed=yes` is set in `[MultiplayerDialogSettings]` and no computer player started with allies. Before, with stock rules, the defeat of one computer player made the remaining computer players ally with each other and turn on the human players.
