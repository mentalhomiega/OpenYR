---
key: ImmuneToPsionics
summary: "Keeps the psychic dominator and mind control weapons from taking this object over."
see_also: [BalloonHover, MindControl, "system:superweapons", "system:mind-control"]
when_omitted:
  kind: context-dependent
  note: A BuildingType starts at yes. Every other type starts at no.
---

Neither the psychic dominator nor a [mind control](/systems/mind-control/) weapon takes over an object whose type is `ImmuneToPsionics=yes`. A mind control weapon does not fire at it.

A structure whose entry has no `ImmuneToPsionics=` line is immune. Write `ImmuneToPsionics=no` on a structure that a mind control weapon should be able to take.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
ImmuneToPsionics=yes
```

The object still takes the dominator blast's damage.
