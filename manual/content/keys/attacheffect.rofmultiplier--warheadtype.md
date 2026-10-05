---
key: AttachEffect.ROFMultiplier
scope: warheadtype
label: Rate of fire multiplier
when_omitted:
  kind: value
  value: "1.0"
  note: "Reload time is unchanged."
---

`AttachEffect.ROFMultiplier` multiplies the reload time that starts after each burst of an object carrying this warhead's effect: `3` makes it fire a third as often and `0.5` twice as often. It is applied with the owner's country `ROF` bonus and before the veteran rate of fire bonus. The delays between the shots of one burst are not changed, and neither is the reload of a weapon with `IsSonic=yes`, or of a weapon with `UseSparkParticles=yes`, `UseFireParticles=yes` or `IsRailgun=yes` while its particle system is active.

```ini title="rulesmd.ini"
[SlowGoo] ; example Warhead
AttachEffect.Duration=300
AttachEffect.ROFMultiplier=3 ; a Grizzly's 60-frame reload becomes about 180
```

:::caution[A long reload outlasts the effect]
Keep `AttachEffect.ROFMultiplier` moderate on short effects. The reload time is set when the reload starts and does not shorten when the effect ends, so a large value can keep the object from firing long after the effect is gone.
:::
