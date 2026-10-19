---
key: UC.DamageMultiplier
summary: "Multiplies the damage of a passed-through shot that is not fatal, before the occupant's own damage rules."
see_also: [UC.PassThrough, UC.FatalRate, "system:garrisons"]
when_omitted:
  kind: value
  value: "100%"
---

A [passed-through](/keys/uc.passthrough/) shot that does not kill an occupant outright hurts one occupant, chosen at random, for the shot's damage times this multiplier, cut to a whole number. The occupant then takes that figure through its own damage rules, such as the warhead's `Verses` against its armor, as any other infantryman does. A multiplier above `100%` raises the damage.

```ini title="rulesmd.ini"
[MYOFFICES] ; example BuildingType
UC.PassThrough=25%
UC.DamageMultiplier=50%
```
