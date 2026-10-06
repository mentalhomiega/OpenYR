---
key: Fighter
summary: Makes an aircraft with a guided weapon fire as it passes its target and fly on, instead of stopping over it.
see_also: ["Landable", "Ammo", "Dock"]
when_omitted:
  kind: value
  value: "no"
---

`Fighter=yes` makes an aircraft attack by flying past its target. It heads for a firing position near the target, fires as soon as the target is in range and ahead of it, holds its heading for the weapon's [`ROF`](/keys/rof/) delay, then picks its next firing position if it has ammunition left. It does not slow down near the firing position while it has ammunition. Without the key, the aircraft slows to a stop at the firing position, turns to face the target and fires from a hover.

The key matters only for an aircraft whose primary weapon fires a guided projectile, such as a Harrier's missile. An aircraft whose projectile flies straight and visibly, such as a bomb, already makes a strafing run over the target whether or not it is a fighter. [Attack passes](/systems/aircraft-operations/#attack-passes) describes the three styles.

```ini title="rules.ini"
[ORCA]
Primary=Maverick
Ammo=1
Fighter=yes   ; fires its missile on approach and flies on
```
