---
key: EffectsRequireVerses
summary: "With no, the extra effects of a warhead reach an object whose armor has a Verses of 0%."
see_also: [EffectsRequireDamage, KillDriver, "system:attach-effects", "system:warheads"]
when_omitted:
  kind: value
  value: "yes"
---

The extra effects of a warhead are the driver kill of [`KillDriver=yes`](/keys/killdriver/) and the effect that [`AttachEffect.Duration`](/keys/attacheffect.duration/) defines. With `EffectsRequireVerses=yes` they reach only an object whose armor the warhead's `Verses` does not set to `0%`. With `no` they also reach an object at `0%`, so a warhead can leave an armor undamaged and still affect it. The damage is not changed by the key.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
KillDriver=yes
EffectsRequireVerses=no
```
