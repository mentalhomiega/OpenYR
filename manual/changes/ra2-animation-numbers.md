---
title: Play the animations that Red Alert 2 missions number
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_PLAY_ANIM
  effect: changed
- type: action
  id: TACTION_DO_EXPLOSION
  effect: changed
credit:
- MentalHomiega
---

A Red Alert 2 mission that plays an animation or sets off an explosion at a waypoint now reads the animation or weapon number against Red Alert 2's `[Animations]` and `[WeaponTypes]` lists. Before, it read the number against Yuri's Revenge's lists, which are longer, so the Boot Camp arrows that point at the first soldier and the way out of the base played other animations. The list is read from the rules file `RA2Rules=` names.
