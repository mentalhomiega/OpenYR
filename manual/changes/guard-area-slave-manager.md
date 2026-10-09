---
title: Send an armed slave manager to Guard, not Area Guard
category: fix
release: 0.2.0
targets:
- type: key
  id: GuardArea
  effect: changed
credit: [MentalHomiega]
---

Before, an armed vehicle that manages slaves, such as Yuri's slave miner, went to Area Guard when its house reached `[IQ] GuardArea`. Now it takes Guard, as in gamemd. A vehicle that is itself a slave takes Guard too.
