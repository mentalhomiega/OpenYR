---
title: Choose an infantry death animation by name with InfDeathAnim
category: feature
release: 0.2.0
targets:
- type: key
  id: InfDeathAnim
  effect: added
credit:
- MentalHomiega
---

`InfDeathAnim=` in a warhead section of `rules.ini` names the animation an infantryman killed by the warhead dies with, in place of `InfDeath`. An animation with `MakeInfantry` mutates the soldier, so a mod can define any number of mutations.
