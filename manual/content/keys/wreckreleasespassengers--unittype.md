---
key: WreckReleasesPassengers
scope: unittype
label: Releases passengers when the vehicle becomes a wreck
see_also: [DeathFrames, MaxDeathCounter, Crashable, "system:transports", "system:destruction-and-debris"]
when_omitted:
  kind: value
  value: "yes"
---

A vehicle whose artwork sets [`DeathFrames`](/keys/deathframes/) above `0` becomes a wreck when it is destroyed, and stays on the map until its [`MaxDeathCounter`](/keys/maxdeathcounter/) passes. `WreckReleasesPassengers` decides what happens to its passengers when it becomes a wreck.

- `yes`: the passengers settle as they do when an outright death lifts a vehicle. A passenger that can enter the vehicle's cell gets out, and the rest die. Forced damage, a fall, a height above 208 leptons, or [`Crashable=yes`](/keys/crashable/) kills all of them, as [transports](/systems/transports/#losing-the-transport) describes.
- `no`: the passengers stay aboard the wreck and die when the wreck explodes.

```ini title="rulesmd.ini"
[MYWALKER] ; a UnitType whose art section sets DeathFrames
WreckReleasesPassengers=no ; passengers stay aboard until the wreck explodes
```

The key has no effect on a vehicle without `DeathFrames`. The crew roll, a hijacker's step out and a truck's crate happen when the wreck is created, whatever the value of this key.
