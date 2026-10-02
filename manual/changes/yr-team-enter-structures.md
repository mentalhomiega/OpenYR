---
title: Send computer teams into bunkers and bio reactors
category: feature
release: 0.2.0
targets:
- type: mission
  id: TMISSION_ENTER_TANK_BUNKER
  effect: changed
- type: mission
  id: TMISSION_ENTER_BIO_REACTOR
  effect: changed
- type: mission
  id: TMISSION_ENTER_BATTLE_BUNKER
  effect: changed
- type: system
  id: garrisons
  effect: changed
credit: [Lucas]
---

Computer teams now drive vehicles into Tank Bunkers and send soldiers into Bio Reactors and Battle Bunkers when their scripts say to, as in Yuri's Revenge. Several soldiers sent into the same Battle Bunker or civilian building now all go inside, where only the first used to. A built structure such as the Battle Bunker now keeps its owner when emptied.
