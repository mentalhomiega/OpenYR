---
key: Abductor.MaxHealth
summary: "The most strength a target may have left to be taken aboard by an Abductor weapon."
see_also: [Abductor, Abductor.AbductBelowPercent]
when_omitted:
  kind: value
  value: "0, which sets no limit"
---

An [`Abductor=yes`](/keys/abductor/) weapon refuses a target whose current strength is above this value, and the hit deals its ordinary damage. The value is in strength points. The check is made when the weapon hits, so a target a first shot weakens to the limit is taken by a later one. A value of `0` or below sets no limit. [`Abductor.AbductBelowPercent`](/keys/abductor.abductbelowpercent/) limits the same target by its share of maximum strength, and a target must pass both.

```ini title="rulesmd.ini"
[MyAbductGun] ; example WeaponType
Abductor=yes
Abductor.MaxHealth=200
```
