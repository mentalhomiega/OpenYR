---
key: ImmuneToPsionics
summary: "Keeps the psychic dominator and mind control weapons from taking this object over."
see_also: [BalloonHover, MindControl, "system:superweapons", "system:mind-control"]
when_omitted:
  kind: value
  value: "no"
---

Neither the psychic dominator nor a [mind control](/systems/mind-control/) weapon takes over an object whose type is `ImmuneToPsionics=yes`. A mind control weapon does not fire at it.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
ImmuneToPsionics=yes
```

The object still takes the dominator blast's damage.
