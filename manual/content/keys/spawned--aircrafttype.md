---
key: Spawned
scope: aircrafttype
label: 'Launched by a carrier flag'
no_effect: true
see_also: [Spawns, MissileSpawn, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "no"
---

Marks an AircraftType that carriers launch. It has no effect on its own; an aircraft is carried when another type names it in [`Spawns`](/keys/spawns/#scope-aircrafttype).
