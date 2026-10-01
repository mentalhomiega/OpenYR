---
key: BalloonHover
summary: "Marks a hovering type that the psychic dominator cannot take over."
see_also: [ImmuneToPsionics, "system:superweapons"]
when_omitted:
  kind: value
  value: "no"
---

The psychic dominator does not take over an object whose type is `BalloonHover=yes`. Yuri's Revenge also uses the key for how such objects fly, which is not ported yet.

```ini title="rulesmd.ini"
[MYUNIT] ; example AircraftType
BalloonHover=yes
```

The object still takes the blast's damage.
