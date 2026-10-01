---
key: ElitePBarrelLength
summary: The barrel length of an object's `ElitePrimary` slot, in its art section.
see_also: [PBarrelLength, ElitePrimary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's PBarrelLength= value.
---

`ElitePBarrelLength=` sets the barrel length the [`ElitePrimary`](/keys/eliteprimary/) slot fires with. It works as [`PBarrelLength`](/keys/pbarrellength/) does for the normal slot and is read from the same art section. An object uses the elite slot only at elite rank and only when `ElitePrimary=` names a weapon; otherwise the normal slot and its `PBarrelLength` apply.

```ini title="artmd.ini"
[MYTANK] ; example image section
ElitePBarrelLength=96
```
