---
key: Bunkerable
scope: aircrafttype
label: 'Can enter a bunker'
see_also: [Bunker, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "yes for a VehicleType, no otherwise"
---

With `yes`, the vehicle can enter its owner's [`Bunker=yes`](/keys/bunker/#scope-buildingtype) structure. Set `no` to keep a vehicle out of bunkers. Only vehicles enter bunkers, so the key has no effect on other objects.
