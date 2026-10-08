---
title: Aim the computer's superweapons by its preferred target
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: action
  id: TACTION_PREFERRED_TARGET
  effect: changed
credit:
- MentalHomiega
---

We now use the preferred target that the Preferred target action stores. A computer house's nuke, lightning storm and paradrops aim at the enemy that its first attack team's leader finds for that quarry, unless the quarry is Anything. Before, we stored the quarry and never read it, so these weapons aimed the same way whatever it was. A house with no attack team, or with no enemy the search finds, strikes nothing and keeps the weapon charged.
