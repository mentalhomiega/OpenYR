---
key: DrainMoneyFrameDelay
summary: "The frames between payments from a drained refinery."
see_also: [DrainMoneyAmount, DrainWeapon]
when_omitted:
  kind: value
  value: "30"
---

A drained refinery's owner pays [`DrainMoneyAmount`](/keys/drainmoneyamount/) credits to the drainer's owner on every frame number that divides by this value. With `0` or below nothing is paid.

```ini title="rulesmd.ini"
[CombatDamage]
DrainMoneyFrameDelay=30
```
