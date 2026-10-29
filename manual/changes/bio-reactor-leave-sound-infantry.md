---
title: Play the Bio Reactor leave sound only from InfantryAbsorb structures
category: fix
release: 0.2.0
targets:
- type: key
  id: LeaveBioReactorSound
  effect: changed
credit:
- MentalHomiega
---

The leave sound plays only when an `InfantryAbsorb=yes` structure releases an object, as gamemd does. A structure that holds vehicles through `UnitAbsorb=yes` and is not `InfantryAbsorb=yes` no longer plays it when they come out.
