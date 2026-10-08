---
title: Rate tech centers by their own ion cannon value
category: fix
release: 0.2.0
targets:
- type: key
  id: AIIonCannonTechCenterValue
  effect: added
- type: system
  id: superweapons
  effect: changed
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

We now read `AIIonCannonTechCenterValue` from `[General]` and use it to rate a structure listed in [`BuildTech`](/keys/buildtech/) when a computer house's ion cannon, nuke or lightning storm picks a target. Before, we ignored the key and rated every such structure 4, as any other structure, so the rules file's figure had no effect. The rules are part of a saved game now, so a save from an earlier build is marked incompatible.
