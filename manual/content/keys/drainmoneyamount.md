---
key: DrainMoneyAmount
summary: "The credits a drained refinery's owner pays each time."
see_also: [DrainMoneyFrameDelay, DrainWeapon]
when_omitted:
  kind: value
  value: "30"
---

Each payment from a drained refinery's owner to the drainer's owner, every [`DrainMoneyFrameDelay`](/keys/drainmoneyframedelay/) frames. An owner with less pays what it has.

```ini title="rulesmd.ini"
[CombatDamage]
DrainMoneyAmount=30
```
