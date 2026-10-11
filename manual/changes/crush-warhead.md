---
title: Crush what a dropped vehicle lands on with CrushWarhead
category: fix
release: 0.2.0
targets:
- type: key
  id: CrushWarhead
  effect: added
- type: key
  id: IsLocomotor
  effect: changed
credit:
- MentalHomiega
---

A vehicle that a locomotor warhead's holder lets go of in the air now crushes the soldiers under it and damages other objects with the `CrushWarhead` set in `[CombatDamage]` of `rulesmd.ini`, as Yuri's Revenge does. A let-go vehicle that hovered in place now falls and is destroyed when it lands, where it used to stay in the air. Saves made by earlier builds no longer load, because the rules and objects now save more settings.
