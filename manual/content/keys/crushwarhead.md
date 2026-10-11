---
key: CrushWarhead
summary: "The warhead a vehicle let go of in the air hits objects with when it lands on them."
see_also: [IsLocomotor, C4Warhead, DamageSelf]
when_omitted:
  kind: value
  value: none
---

```ini title="rulesmd.ini"
[CombatDamage]
CrushWarhead=Crush
```

When a vehicle that an [`IsLocomotor=yes`](/keys/islocomotor/) warhead lifted is let go in the air, it lands on whatever stands on its cell. Only the first object on the cell, not counting the vehicle itself, decides what happens:

- A vehicle or structure is left alone.
- A soldier is crushed, and so is every other object after it on the cell, whoever owns them. The holder gets the kill, or its house if the holder is gone.
- Any other object takes damage from this warhead equal to its strength, ignoring its defenses, so it is destroyed whatever its armor. Without `CrushWarhead`, it takes no damage.

The vehicle is destroyed when it lands either way.

The warhead also lifts the usual rule that a blast spares the object that fired it: an explosion from this warhead hurts its own firer, as it does for a firer whose type sets [`DamageSelf=yes`](/keys/damageself/).
