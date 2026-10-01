---
key: EliteSBarrelThickness
summary: The barrel thickness of an object's `EliteSecondary` slot, in its art section.
see_also: [SBarrelThickness, EliteSecondary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's SBarrelThickness= value.
---

`EliteSBarrelThickness=` sets the barrel thickness the [`EliteSecondary`](/keys/elitesecondary/) slot fires with. It works as [`SBarrelThickness`](/keys/sbarrelthickness/) does for the normal slot and is read from the same art section. An object uses the elite slot only at elite rank and only when `EliteSecondary=` names a weapon; otherwise the normal slot and its `SBarrelThickness` apply.

```ini title="artmd.ini"
[MYTANK] ; example image section
EliteSBarrelThickness=4
```

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key. The weapons of its [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) fire with no barrel length or thickness.
