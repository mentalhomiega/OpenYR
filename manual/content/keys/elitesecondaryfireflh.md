---
key: EliteSecondaryFireFLH
summary: The firing offset of an object's `EliteSecondary` slot, in its art section.
see_also: [SecondaryFireFLH, EliteSecondary, "system:firing-geometry"]
when_omitted:
  kind: inherited
  note: The same section's SecondaryFireFLH= value.
---

`EliteSecondaryFireFLH=` sets the firing offset the [`EliteSecondary`](/keys/elitesecondary/) slot fires with. It works as [`SecondaryFireFLH`](/keys/secondaryfireflh/) does for the normal slot and is read from the same art section. An object uses the elite slot only at elite rank and only when `EliteSecondary=` names a weapon; otherwise the normal slot and its `SecondaryFireFLH` apply.

```ini title="artmd.ini"
[MYTANK] ; example image section
EliteSecondaryFireFLH=120,40,80
```
