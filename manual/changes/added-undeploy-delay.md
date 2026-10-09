---
title: Pack a dug-in soldier up after UndeployDelay frames
category: feature
release: 0.2.0
targets:
- type: key
  id: UndeployDelay
  effect: added
- type: key
  id: Deployer
  effect: changed
  scope: infantrytype
credit:
- MentalHomiega
---

We now pack a dug-in soldier up on its own once its `UndeployDelay` frames have passed, as Yuri's Revenge does. A computer player's soldier with an `UndeployDelay` no longer packs up before it moves. Saves made by earlier builds no longer load, because infantry types now save the setting.
