---
key: LaserInnerColor
summary: The color of the thin core line of a laser beam.
see_also: ["IsLaser", "LaserOuterColor", "LaserOuterSpread", "LaserDuration"]
when_omitted:
  kind: value
  value: "0,0,0"
---

`LaserInnerColor` sets the color of the thin core line down the middle of an [`IsLaser=yes`](/keys/islaser/) beam, as red, green and blue values from `0` to `255`.

```ini title="rules.ini"
[MyObeliskRay] ; example WeaponType
IsLaser=yes
LaserInnerColor=255,0,0
```

At the medium and high [detail levels](/keys/detaillevel/#scope-client-settings), a beam with a glow adds the core's color to the pixels beneath it, and a beam without a glow blends the core onto them, one channel at a time. In the blended case only channels above `0` are used; a `0` channel leaves that channel of the picture beneath unchanged. The default `0,0,0` therefore draws no visible core. Either way the core dims with the beam's age, as [`LaserDuration`](/keys/laserduration/) describes.

At the low detail level, the core is drawn as a plain line in this color, darkened where the picture beneath is shaded, so `0,0,0` draws a black line.

:::note[A partial triplet reads as the default]
`LaserInnerColor=255` names one channel where three are needed, so the core keeps its default color and the debug log records the line; [INI syntax](/formats/ini-syntax/#malformed-values) has the rule.
:::
