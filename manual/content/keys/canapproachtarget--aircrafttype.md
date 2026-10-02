---
key: CanApproachTarget
scope: aircrafttype
label: 'Moves toward targets'
see_also: ["system:tank-bunkers"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the object does not move toward a target beyond its weapons' reach. It forgets the target instead, even one the player ordered it to attack. The Mirage Tank uses this to stay hidden where it was placed.

```ini title="rulesmd.ini"
[MYAMBUSHER] ; example VehicleType
CanApproachTarget=no
```

A human player's object that is guarding an area still moves toward targets.
