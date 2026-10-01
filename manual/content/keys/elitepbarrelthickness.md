---
key: ElitePBarrelThickness
summary: The barrel thickness of an object's `ElitePrimary` slot, in its art section.
see_also: [PBarrelThickness, ElitePrimary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's PBarrelThickness= value.
---

`ElitePBarrelThickness=` sets the barrel thickness the [`ElitePrimary`](/keys/eliteprimary/) slot fires with. It works as [`PBarrelThickness`](/keys/pbarrelthickness/) does for the normal slot and is read from the same art section. An object uses the elite slot only at elite rank and only when `ElitePrimary=` names a weapon; otherwise the normal slot and its `PBarrelThickness` apply.

```ini title="artmd.ini"
[MYTANK] ; example image section
ElitePBarrelThickness=6
```
