---
key: CratePromoteSound
summary: "The sound played when the local player collects a veterancy crate."
see_also: ["system:crates"]
when_omitted:
  kind: value
  value: none
---

When the local player collects a veterancy crate (the `Veteran` result), this sound plays at the crate's cell. A crate collected by any other house plays nothing, and the result does not depend on this setting.

```ini title="rulesmd.ini"
[AudioVisual]
CratePromoteSound=CratePromoted
```
