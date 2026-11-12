---
key: V3RocketLazyCurve
scope: global-rules
summary: "With yes the V3 rocket flies a ballistic arc whose pitch follows its distance to the target."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "yes"
---

With `yes` the V3 rocket flies a ballistic arc whose pitch follows its distance to the target. With `no` it holds its pitch until the flat distance to the target is no more than its height above it, then dives. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketLazyCurve=yes
```
