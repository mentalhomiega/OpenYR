---
key: IsIceGrowthEnabled
scope: theater
label: Theater ice
see_also: [IsArctic, IceGrowthEnabled, IceGrowthRate]
when_omitted:
  kind: context-dependent
  note: "`yes` for SNOW, which keeps its original settings; `no` for every other theater."
---

`IsIceGrowthEnabled` turns ice behavior on for a theater. With it off:

- ice never grows, refreezes, or has its edges smoothed;
- ice never cracks or breaks under a vehicle, and explosions never crack it;
- [`IceGrowthEnabled`](/keys/icegrowthenabled/) on a map has no effect.

```ini title="rules.ini"
[DESERT]
IsIceGrowthEnabled=no
```

A theater whose [`Ice1Set`](/keys/ice1set/), [`Ice2Set`](/keys/ice2set/), [`Ice3Set`](/keys/ice3set/) or [`IceShoreSet`](/keys/iceshoreset/) is missing, or names a set with no tiles, has ice behavior off whatever this key says, and the debug log says so when the theater loads. A mod's snow theater that ships no ice artwork is in that case. Without that rule the ice passes would read the tiles that follow the first ice set's number as ice and rewrite them.

The setting also changes the tiles when the theater loads. With it on, the edge tiles of [`Ice1Set`](/keys/ice1set/), [`Ice2Set`](/keys/ice2set/) and [`Ice3Set`](/keys/ice3set/) count as water, so the open edge of an ice sheet is water to anything moving across it. With it off, those tiles keep the land types their artwork declares.
