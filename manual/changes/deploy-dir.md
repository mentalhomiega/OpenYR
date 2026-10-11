---
title: Turn a landing Siege Chopper to DeployDir
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployDir
  effect: added
credit:
- MentalHomiega
---

A `DeployToLand=yes` vehicle now turns to the `DeployDir` set in `[AudioVisual]` of `rulesmd.ini` while it descends to land, as Yuri's Revenge does. The stock rules set `DeployDir=2`, so a Siege Chopper now lands facing east.
