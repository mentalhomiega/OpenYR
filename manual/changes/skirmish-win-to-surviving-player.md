---
title: Give a skirmish win to the surviving player
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_HOUSE_DESTROY_ALL
  effect: changed
credit: [MentalHomiega]
---

When the last computer house of a skirmish or multiplayer match fell, the match reported a loss to the human. The player who is still in the match now wins, as in Yuri's Revenge. When a loss and a win are flagged in the same frame, a campaign reports the loss and a skirmish or multiplayer match reports the win. [Destroy all of](/mapping/actions/taction-house-destroy-all/) follows the same rule when the house it destroys leaves one side standing.
