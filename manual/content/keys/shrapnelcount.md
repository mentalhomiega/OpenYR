---
key: ShrapnelCount
summary: "How many nearby enemies a projectile's ShrapnelWeapon fires at."
see_also: [ShrapnelWeapon]
when_omitted:
  kind: value
  value: "0"
---

A projectile with a [`ShrapnelWeapon`](/keys/shrapnelweapon/) throws this many pieces of it where it hits. A value of `0` has no limit: a piece goes to every enemy in range, and none lands at random. A negative value throws its absolute value minus the number of whole cells between the firer and the impact, so the count shrinks as the firer stands farther away; with no firer it throws three pieces. A negative value that leaves fewer than one piece throws none.

```ini title="rulesmd.ini"
[MyBouncingBolt] ; example Projectile
ShrapnelCount=2
```
