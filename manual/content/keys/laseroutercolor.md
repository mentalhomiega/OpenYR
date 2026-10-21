---
key: LaserOuterColor
summary: The color of the pair of glow lines drawn alongside a laser beam's core.
see_also: ["IsLaser", "LaserInnerColor", "LaserOuterSpread"]
when_omitted:
  kind: value
  value: "0,0,0"
---

The glow is two extra lines drawn one pixel off the beam's core, both in this color. [`LaserOuterSpread`](/keys/laserouterspread/) shifts the color by a random amount on every frame.

`0,0,0` turns the glow off. The color is tested against black before the spread is added, so no `LaserOuterSpread` brings back a glow set to `0,0,0`.

```ini title="rules.ini"
[MyObeliskRay] ; example WeaponType
IsLaser=yes
LaserInnerColor=255,0,0
LaserOuterColor=128,32,32
```

At the medium and high detail levels the glow adds its color to the pixels beneath it, dimming with the beam's age. At the lowest setting both lines are drawn solid in the full color.

:::note[A partial triplet reads as the default]
`LaserOuterColor=128` names one channel where three are needed, so the glow keeps its default color and the debug log records the line; [INI syntax](/formats/ini-syntax/#malformed-values) has the rule.
:::
