---
key: SurvivorDivisor
summary: The credits of a crewed structure's refund that make one survivor, for a house of the side.
see_also: ["system:capture", AlliedSurvivorDivisor, SovietSurvivorDivisor, ThirdSurvivorDivisor, Crewed]
when_omitted:
  kind: computed
  note: '`AlliedSurvivorDivisor`, `SovietSurvivorDivisor` or `ThirdSurvivorDivisor`, for the first, second or third side in `[Sides]`; a side after the third produces no survivors.'
---

This key replaces the `[General]` divisor for one side. The survivor count of a [`Crewed=yes`](/keys/crewed/) structure is its refund divided by the value, rounded down and limited to between 1 and 5, and a captured structure uses twice the value. `0` gives the side's structures no survivors. A negative value counts as omitted.

```ini title="rules.ini"
[FourthSide]
SurvivorDivisor=1000
```

The count is limited to at least 1, so a very large divisor still leaves one survivor; use `0` for none. [Survivors](/systems/capture/#survivors) gives the rest of the rule.
