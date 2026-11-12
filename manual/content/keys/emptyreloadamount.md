---
key: EmptyReloadAmount
summary: "Shots a vehicle with no ammunition left regains when its reload timer runs out."
see_also: ["ReloadAmount", "Reload", "EmptyReload", "Ammo"]
when_omitted:
  kind: inherited
  note: The value of ReloadAmount.
---

An Ares key for vehicles; aircraft ignore it. With `0` or less a vehicle that has used all its ammunition never reloads. The key does not need [`EmptyReload=`](/keys/emptyreload/), which only changes the delay.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
EmptyReloadAmount=0 ; no reload once empty
```
