---
key: ImmuneToPsionics
summary: "Keeps the psychic dominator from taking this object over."
see_also: [BalloonHover, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

The psychic dominator does not take over an object whose type is `ImmuneToPsionics=yes`.

```ini title="rulesmd.ini"
[MYUNIT] ; example VehicleType
ImmuneToPsionics=yes
```

The object still takes the blast's damage.
