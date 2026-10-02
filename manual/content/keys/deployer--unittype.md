---
key: Deployer
scope: unittype
label: 'Fires its area weapon on deploy'
see_also: [AreaFire]
when_omitted:
  kind: value
  value: "no"
---

With `yes`, the Deploy command makes the vehicle fire its [`AreaFire=yes`](/keys/areafire/#scope-weapontype) weapon at its own cell. The Chaos Drone uses this to release its gas. The vehicle uses its primary weapon if that is an area weapon, and otherwise its secondary. Give the vehicle an area weapon: without one, it fires its usual weapon at its own cell.

```ini title="rulesmd.ini"
[MYDRONE] ; example VehicleType
Deployer=yes
Primary=MyGasRelease ; AreaFire=yes
```
