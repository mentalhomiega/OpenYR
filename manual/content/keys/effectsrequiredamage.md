---
key: EffectsRequireDamage
summary: "With yes, the extra effects of a warhead reach an object only when the hit damages it by at least one point."
see_also: [EffectsRequireVerses, KillDriver, "system:attach-effects", "system:warheads"]
when_omitted:
  kind: value
  value: "no"
---

The extra effects of a warhead are the driver kill of [`KillDriver=yes`](/keys/killdriver/) and the effect that [`AttachEffect.Duration`](/keys/attacheffect.duration/) defines. With `EffectsRequireDamage=yes` they reach an object only when the hit's damage after armor and distance is at least `1`. With the default `no` they do not depend on the damage. A healing hit is not affected by the key.

```ini title="rulesmd.ini"
[MyDriverShotWH] ; example Warhead
KillDriver=yes
EffectsRequireDamage=yes
```
