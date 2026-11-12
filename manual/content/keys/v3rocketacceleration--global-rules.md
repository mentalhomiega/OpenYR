---
key: V3RocketAcceleration
scope: global-rules
summary: "Speed the V3 rocket gains each frame while it climbs and cruises."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0.4"
---

Speed the V3 rocket gains each frame while it climbs and cruises. Its speed never passes the aircraft type's `Speed`. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketAcceleration=0.4
```
