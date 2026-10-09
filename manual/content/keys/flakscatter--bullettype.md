---
key: FlakScatter
scope: bullettype
label: Flak scatter
see_also: [Inaccurate, Arcing, BallisticScatter, Inviso]
when_omitted:
  kind: value
  value: "no"
---

`FlakScatter=yes` moves a flak shell's aim point by an amount that grows with the distance to its target. The Flak Track's ground gun, `FlakTProj`, and the anti-air shot of the Flak Cannon and Flak Track, `FlakProj`, both set it. Which rule moves the aim depends on whether the shell is [`Inviso=yes`](/keys/inviso/).

**Visible shells.** A shell with `Inviso=no` that is also [`Arcing=yes`](/keys/arcing/) and [`Inaccurate=yes`](/keys/inaccurate/) draws a number from zero to [`BallisticScatter`](/keys/ballisticscatter/). It multiplies that number by the distance from the firer to the target, then divides by the firing weapon's [`Range`](/keys/range/#scope-weapontype), rounding down. The result, in leptons, is how far the aim moves in a random direction. A target at full range gets the full BallisticScatter. A target close to the firer gets little scatter. The shell is launched at the moved point, so its flight shows the scatter. A visible shell that is not both `Arcing` and `Inaccurate` gets no scatter from this setting.

**Invisible shells.** A shell with `Inviso=yes` moves its aim when it is fired, whether or not it is `Arcing` or `Inaccurate`. It draws a number from zero to twice BallisticScatter and scales it by the same distance over `Range` rule. The target point moves that many leptons in a random direction, and the shell is placed at the moved point. A blast that lands within 128 leptons of an aircraft can still be moved onto it, as [`Inaccurate`](/keys/inaccurate/) describes. An invisible shell that is also `Arcing` and `Inaccurate` also takes the ordinary arc scatter from BallisticScatter.

```ini title="rules.ini"
[MYFLAKGUN] ; a BulletType, registered by a weapon naming it as its Projectile
Image=120MM
Arcing=yes
Inaccurate=yes
FlakScatter=yes
```

Each moved coordinate is truncated to a whole lepton. The height of the aim point does not change.
