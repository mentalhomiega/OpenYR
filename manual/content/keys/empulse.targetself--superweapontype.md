---
key: EMPulse.TargetSelf
scope: superweapontype
summary: "Makes each firing cannon detonate its weapon on itself."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, each chosen cannon explodes its primary weapon's warhead at its own position with that weapon's damage, and no cannon launches at the target cell.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EMPulse.TargetSelf=yes
```

See [Em pulse](/systems/superweapons/#em-pulse).
