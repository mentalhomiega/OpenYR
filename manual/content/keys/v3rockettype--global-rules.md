---
key: V3RocketType
scope: global-rules
label: V3 rocket type
summary: The AircraftType that flies as the V3 rocket.
see_also: ["system:spawned-aircraft", V3RocketPauseFrames, V3Warhead, "Missile.Custom"]
when_omitted:
  kind: value
  value: none
---

`V3RocketType` is a Yuri's Revenge `[General]` key. The aircraft type it names flies as a launched missile with the `V3Rocket` flight settings in `[General]`, the keys that begin with `V3Rocket`, and explodes with [`V3Warhead`](/keys/v3warhead/). With no key, or a name that is no aircraft type, no type uses those settings.

[Missiles](/systems/spawned-aircraft/#missiles) covers the flight. An aircraft type that sets [`Missile.Custom=yes`](/keys/missile.custom/) uses its own `Missile.` keys in place of them.

```ini title="rulesmd.ini"
[General]
V3RocketType=MYV3ROCKET ; example AircraftType
```
