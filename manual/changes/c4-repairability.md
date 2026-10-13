---
title: Require a structure that allows C4 for sabotage
category: fix
release: 0.1.0
targets:
- type: system
  id: capture
  effect: changed
- type: key
  id: CanC4
  effect: changed
- type: key
  id: Repairable
  effect: changed
credit: [ZivDero, dkeeton]
---

Infantry with `C4=yes` or the C4 veteran ability now arm only a structure that [`CanC4`](/keys/canc4/) allows and that is not `InvisibleInGame=yes`. Before, the test was `Repairable=yes`, so an ammo crate, which has `CanC4=no`, could be armed. Clicks, attack and guard conversions, direct missions and arrival all use this test. A player's soldier with this ability now attacks a structure it may not arm, as an armed soldier does. Before, it walked into the structure and was consumed, as it did for barrels. A computer-owned soldier still walks in. A sabotage order against a structure it may not arm is canceled without arming it.
