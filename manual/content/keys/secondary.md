---
key: Secondary
summary: The WeaponType in the object type's second weapon slot.
see_also: ["system:target-selection"]
when_omitted:
  kind: value
  value: none
---

```ini title="rules.ini"
[MYTANK] ; example UnitType
Primary=MyCannon      ; each names its own weapon section
Secondary=MyMachineGun
```

An object fires its second weapon only where a rule picks it: for example against aircraft when its projectile is `AA=yes`, or when the first weapon's warhead has `0%` [`Verses`](/keys/verses/) against the target's armor. [Which weapon the score assumes](/systems/target-selection/#which-weapon-the-score-assumes) lists the rules.

An elite object keeps this weapon: only the first slot switches to [`Elite`](/keys/elite/). A structure fitted with an upgrade that has a second weapon of its own uses the upgrade's weapon in this slot instead.

A laser or sonic weapon in this slot takes some of its settings from the weapon in the first slot, as [`IsLaser`](/keys/islaser/) and [`IsSonic`](/keys/issonic/) describe.

Writing `none` or `<none>` empties the slot. An ID with no section of its own registers a WeaponType with only its defaults.

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key and reads its weapons from a [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) instead.
