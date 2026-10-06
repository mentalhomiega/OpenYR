---
key: BuildTime.MultipleFactory
summary: The build-time multiplier each factory past the first applies to production of this type, in place of MultipleFactory.
see_also: [MultipleFactory, MultipleFactoryCap, "system:production"]
when_omitted:
  kind: inherited
  note: "[General] MultipleFactory."
---

When a house builds this type, each factory past the first multiplies the build time by `BuildTime.MultipleFactory` instead of by [`MultipleFactory`](/keys/multiplefactory/). A value below `0` is the same as leaving the key out and uses `MultipleFactory`. A value of `0` turns the multiplier off for this type, even when `MultipleFactory` is above `0`.

The key applies to infantry, vehicles, aircraft and structures. [`MultipleFactoryCap`](/keys/multiplefactorycap/) still limits how many factories count, and [More than one factory](/systems/production/#more-than-one-factory) shows the arithmetic and where the multiplier falls among the other build-time adjustments.

```ini title="rulesmd.ini"
[ENGINEER] ; example InfantryType
BuildTime.MultipleFactory=0.9 ; each extra barracks shortens its build time to 90%
```
