---
key: V3RocketBodyLength
scope: global-rules
summary: "Distance in leptons from the V3 rocket's centre to its nose."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "256"
---

Distance in leptons from the V3 rocket's centre to its nose. The missile explodes at the nose, and its trail starts from there. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketBodyLength=256
```
