---
key: DeathWeapon.SuppressInfantry
scope: warheadtype
summary: "With yes, infantry this warhead kills set off no death weapon."
see_also: [DeathWeapon.Suppress, DeathWeapon.SuppressVehicles]
when_omitted:
  kind: value
  value: "no"
---

`DeathWeapon.SuppressInfantry` is an Ares key. With `yes`, every infantry unit this warhead kills dies without setting off its [death weapon](/keys/deathweapon/#scope-aircrafttype). With `no`, the warhead still spares the infantry types its [`DeathWeapon.Suppress`](/keys/deathweapon.suppress/) list names. Infantry this warhead only damages keep their death weapon for a later kill by another warhead.

```ini title="rulesmd.ini"
[MySniperWH] ; example Warhead
DeathWeapon.SuppressInfantry=yes
```
