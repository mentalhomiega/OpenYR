---
key: MissileSpawn
scope: aircrafttype
label: 'Carried missile flag'
see_also: [Spawns, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "no"
---

Marks a carried AircraftType as a missile. A launcher releases missiles nine frames apart, and other carried aircraft twenty frames apart. A missile ignores every order once launched. It flies as a missile only when [`V3RocketType`, `DMislType` or `CMislType`](/systems/spawned-aircraft/#missiles) names it; any other type is flown as an aircraft.
