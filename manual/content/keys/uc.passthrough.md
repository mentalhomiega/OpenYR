---
key: UC.PassThrough
summary: "The share of shots at an occupied structure that hit one of its occupants instead of the structure."
see_also: [UC.FatalRate, UC.DamageMultiplier, SubjectToTrenches, CanBeOccupied, "system:garrisons"]
when_omitted:
  kind: value
  value: "0%"
---

Each unforced shot that would damage the structure rolls against this share while the structure holds at least one occupant. A shot that passes through does no damage to the structure; [Shots that reach the occupants](/systems/garrisons/#shots-that-reach-the-occupants) describes what it does to the occupant. An empty structure takes every shot. Write the value as a percentage such as `25%` or as a fraction such as `0.25`; `100%` passes every shot.

```ini title="rulesmd.ini"
[MYOFFICES] ; example BuildingType
CanBeOccupied=yes
UC.PassThrough=25%
```

A projectile with [`SubjectToTrenches=no`](/keys/subjecttotrenches/) passes every shot into a structure whose value is above `0%`.
