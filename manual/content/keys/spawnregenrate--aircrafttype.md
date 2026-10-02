---
key: SpawnRegenRate
scope: aircrafttype
label: 'Lost spawn replacement delay'
see_also: [SpawnReloadRate, "system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0"
---

The number of frames after a carried aircraft is lost, or a carried missile is spent, before the object has a new one. The count starts when the aircraft is destroyed or the missile has taken off.
