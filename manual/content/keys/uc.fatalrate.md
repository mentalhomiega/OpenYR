---
key: UC.FatalRate
summary: "The share of passed-through shots that kill one occupant of the structure outright."
see_also: [UC.PassThrough, UC.DamageMultiplier, "system:garrisons"]
when_omitted:
  kind: value
  value: "0%"
---

A shot that [passes through](/keys/uc.passthrough/) to the occupants rolls against this share. A shot that wins the roll destroys one occupant, chosen at random, whatever the weapon and the occupant's armor. The key has no effect on a structure whose `UC.PassThrough` is `0%`.

```ini title="rulesmd.ini"
[MYOFFICES] ; example BuildingType
UC.PassThrough=25%
UC.FatalRate=10%
```
