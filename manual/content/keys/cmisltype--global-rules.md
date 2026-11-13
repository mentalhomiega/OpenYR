---
key: CMislType
scope: global-rules
label: Boomer missile type
summary: The AircraftType that flies as the Boomer missile.
see_also: ["system:spawned-aircraft", CMislPauseFrames, CMislWarhead, "Missile.Custom"]
when_omitted:
  kind: value
  value: none
---

`CMislType` is a Yuri's Revenge `[General]` key. The aircraft type it names flies as a launched missile with the `CMisl` flight settings in `[General]`, the keys that begin with `CMisl`, and explodes with [`CMislWarhead`](/keys/cmislwarhead/). With no key, or a name that is no aircraft type, no type uses those settings.

[Missiles](/systems/spawned-aircraft/#missiles) covers the flight. An aircraft type that sets [`Missile.Custom=yes`](/keys/missile.custom/) uses its own `Missile.` keys in place of them.

```ini title="rulesmd.ini"
[General]
CMislType=MYBOOMERMISSILE ; example AircraftType
```
