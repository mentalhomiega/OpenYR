---
title: Run Ion Storm start and stop, and their team lines, on the lightning storm
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_ION_STORM_START
  effect: changed
- type: action
  id: TACTION_ION_STORM_STOP
  effect: changed
- type: mission
  id: TMISSION_ION_STORM_START
  effect: changed
- type: mission
  id: TMISSION_ION_STORM_END
  effect: changed
- type: key
  id: IonStormWarning
  effect: changed
- type: key
  id: IonStormDuration
  effect: changed
- type: system
  id: ion-storms
  effect: changed
credit:
- MentalHomiega
---

Action 44, action 45 and the two matching team script lines now call and end the lightning storm, as Yuri's Revenge does, where they ran the ion storm of earlier games. The storm lasts the given number of game frames, where the action took seconds, and it waits `LightningDeferment` frames instead of `IonStormWarning` seconds. The storm has no owning house, so every house loses its radar while it rages.

Nothing starts an ion storm now, and `IonStormWarning` has no effect.
