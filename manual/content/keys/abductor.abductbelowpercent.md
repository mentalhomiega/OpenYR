---
key: Abductor.AbductBelowPercent
summary: "The largest share of its strength a target may have left for an Abductor weapon to take it."
see_also: [Abductor, ImmuneToAbduction]
when_omitted:
  kind: value
  value: "100%"
---

An [`Abductor=yes`](/keys/abductor/) weapon takes a target whose strength divided by its maximum strength is at or below this value. A healthier target is not taken and the hit damages it. Write the value as a percentage such as `50%` or as a fraction such as `0.5`. At `100%` every target passes this test.

```ini title="rulesmd.ini"
[MyAbductGun] ; example WeaponType
Abductor=yes
Abductor.AbductBelowPercent=50%
```
