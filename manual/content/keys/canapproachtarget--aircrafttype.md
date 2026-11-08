---
key: CanApproachTarget
scope: aircrafttype
label: 'Moves toward targets'
see_also: ["system:tank-bunkers"]
when_omitted:
  kind: value
  value: "yes"
---

With `no`, the object does not move toward a target beyond its weapons' reach. It forgets a target it picked for itself instead. The Mirage Tank uses this to stay hidden where it was placed.

```ini title="rulesmd.ini"
[MYAMBUSHER] ; example VehicleType
CanApproachTarget=no
```

An object ordered to attack still moves toward its target, as does a human player's object guarding an area and a computer player's object on a hunt. A Magnetron holding a unit, a vehicle in a Tank Bunker and a passenger of an open-topped transport also stop chasing, whatever this key says.
