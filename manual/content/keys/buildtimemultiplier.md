---
key: BuildTimeMultiplier
summary: Multiplies how long an object of this type takes to build.
when_omitted:
  kind: value
  value: "1.0"
---

`BuildTimeMultiplier` scales the build time of one type. The build time starts from the type's cost and the house's build speed, is multiplied by this value, and then has the low power and extra factory adjustments applied, so `0.5` builds the type twice as fast and `2` half as fast. The time is rounded down to whole game frames after each step.

```ini title="rulesmd.ini"
[MTNK]
BuildTimeMultiplier=0.5 ; a Grizzly builds in half the usual time
```
