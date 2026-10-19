---
key: PreventAttackMove
summary: Keeps a ground move or attack order from becoming an attack-move for this type.
see_also: [Primary, MobileFire]
when_omitted:
  kind: value
  value: "no"
---

A ground move or attack order becomes an attack-move when an [attack-move](/commands/attackmove/) is armed or its key is held, or when Control and Shift are held. With `PreventAttackMove=yes`, an object of this type keeps the plain order instead: a ground move moves, and a ground attack keeps the cell as its target.

The conversion also needs a primary weapon. A type with no primary weapon never takes it, and an aircraft never takes it, whatever its weapons or this key.

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
PreventAttackMove=yes
```
