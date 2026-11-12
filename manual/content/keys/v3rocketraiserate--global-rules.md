---
key: V3RocketRaiseRate
scope: global-rules
summary: "Read from the rules, but nothing in the flight uses it."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "1"
---

Read from the rules, but nothing in the flight uses it. This is a `[General]` key and applies to the aircraft type named by `V3RocketType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
V3RocketRaiseRate=1
```
