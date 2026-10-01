---
key: CrateMoneySound
summary: "The sound played when the local player collects a money crate."
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: none
---

When the local player collects a money crate (the `Money` result), this sound plays at the crate's cell. A crate collected by any other house plays nothing, and the result does not depend on this setting. It also plays when a `Unit` crate gives money because no vehicle could be placed.

```ini title="rulesmd.ini"
[AudioVisual]
CrateMoneySound=CrateMoney
```
