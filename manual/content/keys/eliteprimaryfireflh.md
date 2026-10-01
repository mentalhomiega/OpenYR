---
key: ElitePrimaryFireFLH
summary: The firing offset of an object's `ElitePrimary` slot, in its art section.
see_also: [PrimaryFireFLH, ElitePrimary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's PrimaryFireFLH= value.
---

`ElitePrimaryFireFLH=` sets the firing offset the [`ElitePrimary`](/keys/eliteprimary/) slot fires with. It works as [`PrimaryFireFLH`](/keys/primaryfireflh/) does for the normal slot and is read from the same art section. An object uses the elite slot only at elite rank and only when `ElitePrimary=` names a weapon; otherwise the normal slot and its `PrimaryFireFLH` apply.

```ini title="artmd.ini"
[MYTANK] ; example image section
ElitePrimaryFireFLH=150,0,100
```

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key and reads its weapons from a [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) instead.
