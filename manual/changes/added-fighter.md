---
title: Make Harriers fire on approach with Fighter=yes
category: feature
release: 0.2.0
targets:
- type: key
  id: Fighter
  effect: added
- type: system
  id: aircraft-operations
  effect: changed
credit:
- MentalHomiega
---

`Fighter=yes` on an aircraft type makes an aircraft with a guided weapon fire as it passes its target and fly on, as Yuri's Revenge's Harrier and Black Eagle do. Such aircraft previously stopped over the target, turned to face it and fired from a hover. Saves made by earlier builds no longer load, because aircraft types now save the key.
