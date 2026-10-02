---
key: Spawner
scope: weapontype
label: 'Launches carried aircraft'
see_also: [Spawns, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "no"
---

Firing a weapon with `Spawner=yes` fires no projectile. It sends the firer's carried aircraft or missiles from [`Spawns`](/keys/spawns/#scope-aircrafttype) at the target instead. A firer that carries none fires nothing. The weapon's `Range` and `ROF` still decide when the firer attacks.

```ini title="rulesmd.ini"
[MyDroneLauncher] ; example Weapon
Spawner=yes
Range=20
ROF=150
Projectile=Invisible
Warhead=Special
```
