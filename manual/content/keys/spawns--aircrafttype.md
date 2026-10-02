---
key: Spawns
scope: aircrafttype
label: 'Carried aircraft or missile type'
see_also: [SpawnsNumber, Spawner, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: none
---

An object whose type names an AircraftType in `Spawns` and sets [`SpawnsNumber`](/keys/spawnsnumber/#scope-aircrafttype) above `0` carries that many of it from the moment it enters the map. Its weapons with [`Spawner=yes`](/keys/spawner/#scope-weapontype) send them at the target instead of firing a projectile. [Spawned aircraft and missiles](/systems/spawned-aircraft/) covers the launch, attack and return cycle.

```ini title="rulesmd.ini"
[MYCARRIER] ; example VesselType
Spawns=MYDRONE
SpawnsNumber=3
SpawnRegenRate=500
SpawnReloadRate=200
```
