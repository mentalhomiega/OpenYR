---
key: Abductor.Anim
summary: "The animation an Abductor weapon plays where it takes its target from."
see_also: [Abductor]
when_omitted:
  kind: value
  value: "none"
---

When an [`Abductor=yes`](/keys/abductor/) weapon takes a target aboard, this animation plays at the centre of the target. It plays only when the target is taken, not when the hit deals damage instead.

```ini title="rulesmd.ini"
[MyAbductGun] ; example WeaponType
Abductor=yes
Abductor.Anim=WARPAWAY
```
