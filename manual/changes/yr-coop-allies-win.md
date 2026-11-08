---
title: Win a Cooperative match when any living person's side is left
category: fix
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
credit:
- MentalHomiega
---

In a Cooperative game against other machines, a defeated player now wins when every house still alive is allied with every other and at least one of them is a person. Before, the player won only when exactly one living house was left. A skirmish set to Cooperative no longer gives a defeated player the win, as in Yuri's Revenge. See [the multiplayer mode](/formats/spawn-ini/#the-multiplayer-mode).
