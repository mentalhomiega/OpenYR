---
key: ShowTimer
summary: "Shows every player this superweapon's countdown on the battlefield."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

While a house holds a superweapon of this type, every player sees [its countdown](/systems/superweapons/#countdown-timers) in the bottom right corner of the battlefield.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
ShowTimer=yes
```
