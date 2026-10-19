---
key: Money.Amount
scope: superweapontype
summary: "Charges or pays the house each time the weapon fires."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "0"
---

A negative amount is taken from the house's credits when the weapon launches, using stored ore when credits run short. A positive amount is paid to it. When a house cannot cover a negative amount, the shot is refused, the weapon stays charged, and the player hears [`EVA.InsufficientFunds=`](/keys/eva.insufficientfunds/) and reads [`Message.InsufficientFunds=`](/keys/message.insufficientfunds/). A charge-draining weapon pays when it is switched on.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
Money.Amount=-1000
```

See [Cost and timing](/systems/superweapons/#cost-and-timing).
