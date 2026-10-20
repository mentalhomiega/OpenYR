---
key: AllowZeroDamage
scope: warheadtype
summary: "Lets a blast with no damage still reach the objects in its spread."
see_also: [Flash.Duration, EffectsRequireDamage, CellSpread]
when_omitted:
  kind: value
  value: "no, so a blast with no damage does nothing"
---

With `AllowZeroDamage=yes`, an explosion of this warhead that carries 0 damage still hits every object within its [`CellSpread=`](/keys/cellspread/), and a hit of 0 stays at 0 against an unforced target instead of being raised to 1. The warhead's effects, such as [`Flash.Duration`](/keys/flash.duration/), apply to the objects it reaches, so a warhead can act without hurting anything. With `no`, an explosion of 0 damage is dropped before it reaches any object, as it was before.

```ini title="rulesmd.ini"
[MyEffectWH] ; example WarheadType
CellSpread=3
Flash.Duration=40
AllowZeroDamage=yes
```
