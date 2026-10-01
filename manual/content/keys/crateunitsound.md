---
key: CrateUnitSound
summary: "The sound played when the local player collects a crate that gives a vehicle."
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: none
---

When the local player collects a crate that gives a vehicle (the `Unit` result), this sound plays at the crate's cell. A crate collected by any other house plays nothing, and the result does not depend on this setting. It plays only when the vehicle is placed; a crate that gives money instead plays [`CrateMoneySound`](/keys/cratemoneysound/).

```ini title="rulesmd.ini"
[AudioVisual]
CrateUnitSound=CrateFreeUnit
```
