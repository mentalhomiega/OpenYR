---
key: Sidebar.YuriFileNames
summary: Whether the side's radar logo is drawn from RADARY.SHP in the colors of RADARYURI.PAL.
see_also: ["system:sidebar", "Sidebar.MixFileIndex"]
when_omitted:
  kind: computed
  note: Yes for the third side in `[Sides]` and no for every other side.
---

With `yes`, the radar logo that fills the radar's place while the radar is off, and its switch-on animation, are drawn from `RADARY.SHP` in the colors of `RADARYURI.PAL`. Both files come from the side's sidebar archives, so a side that sets the key needs them there. With `no`, or when either file is missing, the logo is `RADAR.SHP` in the colors of the sidebar palette.

```ini title="rules.ini"
[Nod]
Sidebar.YuriFileNames=yes
```

The key does not change which archives are mounted; [`Sidebar.MixFileIndex`](/keys/sidebar.mixfileindex/) does that. The classic sidebar and the modern panel draw the same logo.
