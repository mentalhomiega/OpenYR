---
key: V3RocketAltitude
scope: global-rules
summary: "Height above the ground, in leptons, at which the V3 rocket stops climbing and starts to cruise."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "768"
---

Height above the ground, in leptons, at which the V3 rocket stops climbing and starts to cruise. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketAltitude=768
```
