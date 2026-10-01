---
key: EliteSBarrelLength
summary: The barrel length of an object's `EliteSecondary` slot, in its art section.
see_also: [SBarrelLength, EliteSecondary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's SBarrelLength= value.
---

`EliteSBarrelLength=` sets the barrel length the [`EliteSecondary`](/keys/elitesecondary/) slot fires with. It works as [`SBarrelLength`](/keys/sbarrellength/) does for the normal slot and is read from the same art section. An object uses the elite slot only at elite rank and only when `EliteSecondary=` names a weapon; otherwise the normal slot and its `SBarrelLength` apply.

```ini title="artmd.ini"
[MYTANK] ; example image section
EliteSBarrelLength=64
```

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key. The weapons of its [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) fire with no barrel length or thickness.
