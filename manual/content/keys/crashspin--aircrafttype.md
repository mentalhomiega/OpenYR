---
key: CrashSpin
scope: aircrafttype
label: Spins while crashing
see_also: [Crashable]
when_omitted:
  kind: value
  value: "yes"
---

`CrashSpin` is an Ares key. With `CrashSpin=no`, an aircraft destroyed in the air falls to the ground without rolling or pitching: it keeps the attitude it had when it was hit. With `yes`, the crash sets it rolling to one side at a random rate of 0.10 to 0.25 radians a frame and tipping forward at up to 0.1 radians a frame until it hits the ground.

The key changes only how the wreck is drawn. Where it falls, the damage it does on impact and the sounds it plays are the same. An aircraft with [`Crashable=no`](/keys/crashable/#scope-aircrafttype) does not fall, so the key does nothing for it. A vehicle with a jumpjet locomotor ignores the key.

```ini title="rulesmd.ini"
[ORCA] ; example AircraftType
CrashSpin=no
```
