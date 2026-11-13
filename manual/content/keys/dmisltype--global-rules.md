---
key: DMislType
scope: global-rules
label: Dreadnought missile type
summary: The AircraftType that flies as the Dreadnought missile.
see_also: ["system:spawned-aircraft", DMislPauseFrames, DMislWarhead, "Missile.Custom"]
when_omitted:
  kind: value
  value: none
---

`DMislType` is a Yuri's Revenge `[General]` key. The aircraft type it names flies as a launched missile with the `DMisl` flight settings in `[General]`, the keys that begin with `DMisl`, and explodes with [`DMislWarhead`](/keys/dmislwarhead/). With no key, or a name that is no aircraft type, no type uses those settings.

[Missiles](/systems/spawned-aircraft/#missiles) covers the flight. An aircraft type that sets [`Missile.Custom=yes`](/keys/missile.custom/) uses its own `Missile.` keys in place of them.

```ini title="rulesmd.ini"
[General]
DMislType=MYDREADMISSILE ; example AircraftType
```
