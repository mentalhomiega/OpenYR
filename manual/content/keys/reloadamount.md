---
key: ReloadAmount
summary: "Shots a vehicle regains each time its reload timer runs out."
see_also: ["EmptyReloadAmount", "Reload", "EmptyReload", "Ammo"]
when_omitted:
  kind: value
  value: "1"
---

An Ares key for vehicles; aircraft ignore it. Each time the [`Reload=`](/keys/reload/) timer of a vehicle that is short of ammunition runs out, it regains this many shots, never more than [`Ammo=`](/keys/ammo/), and the timer starts again. A negative value takes shots away, down to 0. A vehicle with no ammunition left uses [`EmptyReloadAmount=`](/keys/emptyreloadamount/) instead, and [`EmptyReload=`](/keys/emptyreload/) still sets the delay in that case.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Ammo=10
Reload=60
ReloadAmount=2
```
