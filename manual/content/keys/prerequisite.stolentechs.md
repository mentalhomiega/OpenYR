---
key: Prerequisite.StolenTechs
summary: The kinds of stolen technology a house must have before it can build this type.
see_also: ["SpyEffect.StolenTechIndex", "SpyEffect.Custom", Prerequisite, "system:production"]
when_omitted:
  kind: value
  value: "none"
---

`Prerequisite.StolenTechs` is a list of numbers from 0 to 31. A house can build the type only once it has stolen every listed kind, by infiltrating a structure whose [`SpyEffect.StolenTechIndex`](/keys/spyeffect.stolentechindex/) lists it. The other build rules still apply. A number outside 0 to 31 is ignored, and an empty or absent list adds no requirement.

```ini title="rulesmd.ini"
[MTNK] ; example VehicleType
Prerequisite.StolenTechs=0,3
```
