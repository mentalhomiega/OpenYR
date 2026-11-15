---
key: DMislAltitude
scope: global-rules
summary: "Height above the ground, in leptons, at which the Dreadnought missile stops climbing and starts to cruise."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "768"
---

Height above the ground, in leptons, at which the Dreadnought missile stops climbing and starts to cruise. This is a `[General]` key and applies to the aircraft type named by `DMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
DMislAltitude=768
```
