---
key: BallisticScatter.Max
summary: "The most a projectile type moves the aim point of an inaccurate arcing shot, in cells."
see_also: [BallisticScatter.Min, BallisticScatter, Inaccurate, Arcing, FlakScatter]
when_omitted:
  kind: value
  value: "the `[CombatDamage]` `BallisticScatter` value"
---

An [`Inaccurate=yes`](/keys/inaccurate/) and [`Arcing=yes`](/keys/arcing/) shell moves its aim point in a random direction by a distance drawn between [`BallisticScatter.Min`](/keys/ballisticscatter.min/) and this value. Write the value in cells, as in `0.75`. Setting `Min` and `Max` to the same value gives every shot that exact scatter, and `0` for both gives no scatter.

```ini title="rulesmd.ini"
[MyShell] ; example BulletType
Inaccurate=yes
Arcing=yes
BallisticScatter.Max=2.5
```
