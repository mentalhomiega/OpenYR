---
key: EMPulse.PulseDelay
scope: superweapontype
summary: "Reads how long a cannon waits before it fires; it has no effect yet."
see_also: ["system:superweapons"]
when_omitted:
  kind: value
  value: "32"
---

The value is stored. Cannons keep the stock delay.

```ini title="rulesmd.ini"
[MySpecial] ; example SuperWeaponType
EMPulse.PulseDelay=32
```
