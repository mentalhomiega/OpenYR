---
key: GuardModeStray
summary: Distance in cells an object in Guard Area may stray from the unit it guards before it returns.
see_also: [GuardRange, "system:target-selection"]
when_omitted:
  kind: value
  value: "2"
  note: "512 leptons."
---

An object in Guard Area whose home is another infantryman, vehicle or aircraft drops its target and heads back to that unit once it is more than `GuardModeStray` from it, provided it is neither firing nor already moving somewhere. The value is in cells, and fractions are accepted.

An object guarding a spot on the map uses its [area radius](/keys/guardrange/) instead.

```ini title="rulesmd.ini"
[General]
GuardModeStray=3 ; escorts may chase targets up to 3 cells from the unit they guard
```
