---
key: BallisticScatter.Min
summary: "The least a projectile type moves the aim point of an inaccurate arcing shot, in cells."
see_also: [BallisticScatter.Max, BallisticScatter, Inaccurate, Arcing, FlakScatter]
when_omitted:
  kind: context-dependent
  note: "Half of `[CombatDamage]` `BallisticScatter` for a shell that is not `FlakScatter=yes` or is `Inviso=yes`, otherwise `0`."
---

An [`Inaccurate=yes`](/keys/inaccurate/) and [`Arcing=yes`](/keys/arcing/) shell moves its aim point in a random direction by a distance drawn between this value and [`BallisticScatter.Max`](/keys/ballisticscatter.max/). A `Max` lower than `Min` is raised to it. A [`FlakScatter=yes`](/keys/flakscatter/) shell that is not `Inviso=yes` draws a share between the two and scales it by its distance to the target over its weapon's range. The extra scatter that an invisible flak shell takes when it is placed is not changed by the key.

```ini title="rulesmd.ini"
[MyShell] ; example BulletType
Inaccurate=yes
Arcing=yes
BallisticScatter.Min=0.5
BallisticScatter.Max=0.75
```
