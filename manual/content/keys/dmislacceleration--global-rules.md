---
key: DMislAcceleration
scope: global-rules
summary: "Speed the Dreadnought missile gains each frame while it climbs and cruises."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "0.4"
---

Speed the Dreadnought missile gains each frame while it climbs and cruises. Its speed never passes the aircraft type's `Speed`. This is a `[General]` key and applies to the aircraft type named by `DMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
DMislAcceleration=0.4
```
