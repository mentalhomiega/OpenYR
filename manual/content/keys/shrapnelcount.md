---
key: ShrapnelCount
summary: "How many nearby enemies a projectile's ShrapnelWeapon fires at."
see_also: [ShrapnelWeapon]
when_omitted:
  kind: value
  value: "0"
---

A projectile with a [`ShrapnelWeapon`](/keys/shrapnelweapon/) fires it at up to this many objects around the point where it hits. With `0` or below it fires none.

```ini title="rulesmd.ini"
[MyBouncingBolt] ; example Projectile
ShrapnelCount=2
```
